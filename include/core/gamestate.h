#pragma once
#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <mana/core/memoryallocator.h>
//
#include <cstorage/cstorage.h>

#include "core/dimensions.h"
#include "core/entity.h"
#include "core/position.h"
#include "core/scenery.h"
#include "gamestage.h"

struct GameState {
  struct GameStage* game_stage;

  struct ComponentRegistry position_registry;
  struct ComponentRegistry dimensions_registry;
  struct ComponentRegistry scenery_registry;
  struct Vector entities;
};

struct GameStage;

int game_state_init(struct GameState* game_state);
void game_state_delete(struct GameState* game_state);
void game_state_update(struct GameState* game_state, double delta_time);

#endif  // GAME_STATE_H