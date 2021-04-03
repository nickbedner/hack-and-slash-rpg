#pragma once
#ifndef PLAYER_CAMERA_H
#define PLAYER_CAMERA_H

#include <mana/core/memoryallocator.h>
//
#include <core/entity.h>
#include <mana/graphics/utilities/camera.h>
#include <mana/mana.h>

struct PlayerCamera {
  struct Camera camera;
  struct Entity* focus_entity;

  float max_camera_velocity;
  float camera_left_right_velocity;
  float camera_in_out_velocity;
  float camera_up_down_velocity;
  float y_find;
};

int player_camera_init(struct PlayerCamera* player_camera);
void player_camera_update(struct PlayerCamera* player_camera, float delta_time);

#endif  // PLAYER_CAMERA_H
