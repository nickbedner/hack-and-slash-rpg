#include "state/game.h"

void game_init(struct Game* game, struct Mana* mana, struct Window* window) {
  struct GPUAPI* gpu_api = &mana->engine.gpu_api;
  game->window = window;

  // Connect to server or start local server
  game->game_state = calloc(1, sizeof(struct GameState));
  game_state_init(game->game_state);

  // Create FXAA shader, off be default
  fxaa_shader_init(&game->fxaa_shader, gpu_api);
  game->fxaa_shader.on = 0;
  // Create static and dynamic sprite shaders
  sprite_shader_init(&game->sprite_shader, gpu_api, 0);
  sprite_animation_shader_init(&game->sprite_animation_shader, gpu_api, 0);

  // Create camera to follow entities
  player_camera_init(&game->player_camera);

  // Init ecs
  component_registry_init(&game->render_registry, sizeof(struct Render));

  // Start job system for multithreading, on standby
  job_system_init(&game->job_system);

  // TODO: Implement async asset loading
  // Load game assets
  resource_manager_init(&game->resource_manager, gpu_api);

  struct Noise noise = {0};
  int size = 32;
  float STEP = 1.0f / size;
  //noise.noise_type = SPHERE_NOISE;
  //sphere_noise_init(&noise.sphere_noise);
  //noise.sphere_noise.step = STEP;
  //noise.sphere_noise.sphere_origin[0] = size / 2;
  //noise.sphere_noise.sphere_origin[1] = size / 2;
  //noise.sphere_noise.sphere_origin[2] = size / 2;
  noise.noise_type = RIDGED_FRACTAL_NOISE;
  ridged_fractal_noise_init(&noise.ridged_fractal_noise);
  noise.ridged_fractal_noise.octave_count = 4;
  noise.ridged_fractal_noise.frequency = 1.0;
  noise.ridged_fractal_noise.lacunarity = 2.2324f;
  noise.ridged_fractal_noise.step = STEP;

  struct Vector* noises = calloc(1, sizeof(struct Vector));
  vector_init(noises, sizeof(struct Noise));
  vector_push_back(noises, &noise);

  manifold_dual_contouring_shader_init(&game->planet_shader, &mana->engine.gpu_api);
  manifold_planet_init(&game->planet, &mana->engine.gpu_api, size, &game->planet_shader, noises, (vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f});
}

void game_delete(struct Game* game, struct Mana* mana) {
  struct GPUAPI* gpu_api = &mana->engine.gpu_api;
  // Wait for command buffers to finish before deleting desciptor sets
  vkWaitForFences(gpu_api->vulkan_state->device, 2, gpu_api->vulkan_state->swap_chain->in_flight_fences, VK_TRUE, UINT64_MAX);

  // Free sprite data allocated on gpu
  char* render_list_key = NULL;
  struct MapIter render_list_iter = map_iter();
  while ((render_list_key = map_next(&game->render_registry.registry, &render_list_iter)))
    sprite_delete(&((struct Render*)map_get(&game->render_registry.registry, render_list_key))->sprite, gpu_api);

  vector_delete(&game->planet.noises);
  free(&game->planet.noises);

  manifold_planet_delete(&game->planet, gpu_api);
  manifold_dual_contouring_shader_delete(&game->planet_shader, gpu_api);

  // Delete ecs
  component_registry_delete(&game->render_registry);

  // Delete shaders
  sprite_animation_shader_delete(&game->sprite_animation_shader, gpu_api);
  sprite_shader_delete(&game->sprite_shader, gpu_api);
  fxaa_shader_delete(&game->fxaa_shader, gpu_api);

  // Delete resources
  resource_manager_delete(&game->resource_manager, gpu_api);

  // Delete job system
  job_system_delete(&game->job_system);
}

void game_update(struct Game* game, struct Mana* mana, double delta_time) {
  struct GPUAPI* gpu_api = &mana->engine.gpu_api;
  game_check_for_window_resize(game, gpu_api);

  // Update camera and user input
  player_camera_update(&game->player_camera, delta_time);
  game_update_input(game, &mana->engine);

  // Enqueue and process jobs
  game_update_jobs(game, gpu_api);

  // Build and sort list of entities to render
  struct ArrayList sorted_render_list = {0};
  array_list_init(&sorted_render_list);
  game_sort_render_entites(game, gpu_api, &sorted_render_list);

  // Update projection and view matrices
  gpu_api->vulkan_state->gbuffer->projection_matrix = camera_get_projection_matrix(&game->player_camera.camera, game->window);
  gpu_api->vulkan_state->gbuffer->view_matrix = camera_get_view_matrix(&game->player_camera.camera);

  // TODO: Implement instanced rendering and texture atlas
  // Start blitting to the gbuffer
  gbuffer_start(gpu_api->vulkan_state->gbuffer, gpu_api->vulkan_state);
  game_render_entities(game, gpu_api, &sorted_render_list);

  manifold_planet_update_uniforms(&game->planet, gpu_api, &game->player_camera, (vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f});
  manifold_planet_render(&game->planet, gpu_api);

  gbuffer_stop(gpu_api->vulkan_state->gbuffer, gpu_api->vulkan_state);
  // Free memory allocated inside arraylist
  array_list_delete(&sorted_render_list);

  // Blit gbuffer to post process ping pong
  blit_post_process_render(gpu_api->vulkan_state->post_process->blit_post_process, gpu_api);
  fxaa_shader_render(&game->fxaa_shader, gpu_api);
  // Blit post process to swap chain
  blit_swap_chain_render(gpu_api->vulkan_state->swap_chain->blit_swap_chain, gpu_api);
}

void game_update_input(struct Game* game, struct Engine* engine) {
  // If the window is not in focues, ignore user input
  if (glfwGetWindowAttrib(engine->graphics_library.glfw_library.glfw_window, GLFW_FOCUSED) == 0)
    return;

  struct InputManager* input_manager = game->window->input_manager;

  // Signals window to close, will call delete functions
  if (input_manager->keys[GLFW_KEY_ESCAPE].state == PRESSED)
    glfwSetWindowShouldClose(engine->graphics_library.glfw_library.glfw_window, 1);

  // Toggle FXAA
  if (input_manager->keys[GLFW_KEY_1].pushed == 1)
    (game->fxaa_shader.on ^= 1) ? printf("FXAA ON\n") : printf("FXAA OFF\n");

  // Reload scenery from xml
  if (input_manager->keys[GLFW_KEY_2].pushed == 1)
    game_hotswap_scenery(game, &engine->gpu_api);

  // Change master volume levels
  if (input_manager->keys[GLFW_KEY_O].pushed == 1) {
    game->resource_manager.audio_manager.master_volume -= 0.1f;
    if (game->resource_manager.audio_manager.master_volume < 0.0f)
      game->resource_manager.audio_manager.master_volume = 0.0f;
  }
  if (input_manager->keys[GLFW_KEY_P].pushed == 1) {
    game->resource_manager.audio_manager.master_volume += 0.1f;
    if (game->resource_manager.audio_manager.master_volume > 1.0f)
      game->resource_manager.audio_manager.master_volume = 1.0f;
  }

  // Zoom camera in/out
  if (input_manager->keys[GLFW_KEY_Z].state == PRESSED)
    game->player_camera.camera.position.z += 0.01f;
  if (input_manager->keys[GLFW_KEY_X].state == PRESSED)
    game->player_camera.camera.position.z -= 0.01f;

  // Move camera up/down
  if (input_manager->keys[GLFW_KEY_E].state == PRESSED)
    game->player_camera.camera.position.y += 0.01f;
  if (input_manager->keys[GLFW_KEY_Q].state == PRESSED)
    game->player_camera.camera.position.y -= 0.01f;
}
