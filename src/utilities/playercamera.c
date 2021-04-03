#include "utilities/playercamera.h"

int player_camera_init(struct PlayerCamera* player_camera) {
  camera_init(&player_camera->camera);
  vec3 added_pos = vec3_scale(player_camera->camera.front, -6.9f);
  player_camera->camera.position = vec3_add(player_camera->camera.position, added_pos);
  // Note: Camera and player y starts at -0.5
  player_camera->y_find = -0.5f;  //player_camera->camera.position.y;

  player_camera->max_camera_velocity = 0.024f;
  player_camera->camera_left_right_velocity = 0.0f;
  player_camera->camera_in_out_velocity = 0.0f;
  player_camera->camera_up_down_velocity = 0.0f;

  return 0;
}

void player_camera_update(struct PlayerCamera* player_camera, float delta_time) {
  //float camera_mov_diff_x = (player_camera->camera.position.x - player_camera->focus_entity->position.x) * 4.0f * delta_time;
  //player_camera->camera.position.x -= camera_mov_diff_x;
  //float camera_mov_diff_y = (player_camera->camera.position.y - player_camera->y_find) * 4.0f * delta_time;
  //player_camera->camera.position.y -= camera_mov_diff_y;

  //if (player_camera->focus_entity->position.y > -0.5)
  //  player_camera->y_find = player_camera->focus_entity->position.y;
  //else
  //  player_camera->y_find = -0.5f;

  camera_update_vectors(&player_camera->camera);
}

/*// TODO: Add mouse let go no skip
void game_update_camera(struct Game* game, struct Engine* engine) {
  // TODO: Get all inputs and store
  struct InputManager* input_manager = game->window->input_manager;

  int in_focus = glfwGetWindowAttrib(engine->graphics_library.glfw_library.glfw_window, GLFW_FOCUSED);
  if (in_focus == 0) {
    //game->camera.prev_yaw = game->camera.prev_pitch = game->camera.pitch = game->camera.prev_roll = game->camera.roll = 0.0f;
    return;
  }

  int mouse_locked = (glfwGetMouseButton(engine->graphics_library.glfw_library.glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) ? 1 : 0;
  mouse_locked ? glfwSetInputMode(engine->graphics_library.glfw_library.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED) : glfwSetInputMode(engine->graphics_library.glfw_library.glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  GLdouble x_pos, y_pos;
  glfwGetCursorPos(engine->graphics_library.glfw_library.glfw_window, &x_pos, &y_pos);

  double x_diff = input_manager->last_mouse_x_pos - x_pos;
  double y_diff = input_manager->last_mouse_y_pos - y_pos;

  input_manager->last_mouse_x_pos = x_pos;
  input_manager->last_mouse_y_pos = y_pos;

  // Handles mouse held and big jump edge cases
  if ((mouse_locked == 0 && game->camera.mouse_locked == 1) || (mouse_locked == 0 && (x_pos <= 0 || y_pos <= 0 || x_pos >= game->window->width || y_pos >= game->window->height)) || (fabs(x_diff) > (game->window->width / 2.0f) || fabs(y_diff) > (game->window->height / 2.0f)))
    x_diff = y_diff = 0;

  x_diff *= game->camera.sensitivity;
  y_diff *= game->camera.sensitivity;

  game->camera.mouse_locked = mouse_locked;
}*/
