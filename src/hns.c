#include "hns.h"

int hns_init(struct HNS* hns) {
  int mana_init_error = mana_init(&hns->mana, (struct EngineSettings){GLFW_LIBRARY, VULKAN_API});
  switch (mana_init_error) {
    case (MANA_SUCCESS):
      break;
    case (MANA_ENGINE_ERROR):
      fprintf(stderr, "Failed to setup glfw for engine!\n");
      return HNS_MANA_ERROR;
    default:
      fprintf(stderr, "Unknown mana error! Error code: %d\n", mana_init_error);
      return HNS_MANA_ERROR;
  }
  // TODO: Load from settings or on first boot do fullscreen and monitor resolution
  // Note: Handy function to get max msaa samples supported by gpu
  //vulkan_renderer_get_max_usable_sample_count(gpu_api)
  window_init(&hns->window, &hns->mana.engine, 1280, 720, 1);
  window_set_icon(&hns->window, "./assets/textures/etc/icon.png");

  hns_change_state(hns, STATE_GAME);

  return HNS_SUCCESS;
}

void hns_delete(struct HNS* hns) {
  //game_delete(&hns->game, &hns->mana);
  window_delete(&hns->window);
  mana_cleanup(&hns->mana);
}

void hns_start(struct HNS* hns) {
  struct Engine* engine = &hns->mana.engine;

  while (window_should_close(&hns->window) == 0) {
    engine->fps_counter.now_time = engine_get_time();
    engine->fps_counter.delta_time = engine->fps_counter.now_time - engine->fps_counter.last_time;
    engine->fps_counter.last_time = engine->fps_counter.now_time;

    window_prepare_frame(&hns->window);
    (hns->state.state_func)(hns->state.state, &hns->mana, engine->fps_counter.delta_time);
    //game_update(&hns->game, &hns->mana, engine->fps_counter.delta_time);
    window_end_frame(&hns->window);

    // TODO: Check for state change

    engine->fps_counter.frames++;

    if (engine_get_time() - engine->fps_counter.timer > 1.0) {
      engine->fps_counter.timer++;

      engine->fps_counter.second_target_fps = engine->fps_counter.target_fps;
      engine->fps_counter.second_average_fps = engine->fps_counter.average_fps;
      engine->fps_counter.second_frames = engine->fps_counter.frames;

      char title_buffer[1024];
      sprintf(title_buffer, "HNS %d", (int)engine->fps_counter.second_frames);
      window_set_title(&hns->window, title_buffer);

      engine->fps_counter.fps = engine->fps_counter.frames;

      float average_calc = 0;
      for (int loopNum = FPS_COUNT - 1; loopNum >= 0; loopNum--) {
        if (loopNum != 0)
          engine->fps_counter.fps_past[loopNum] = engine->fps_counter.fps_past[loopNum - 1];

        average_calc += engine->fps_counter.fps_past[loopNum];
      }
      engine->fps_counter.fps_past[0] = engine->fps_counter.fps;
      engine->fps_counter.average_fps = average_calc / FPS_COUNT;
      engine->fps_counter.frames = 0;
    }
  }
}