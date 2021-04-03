#include "core/gamestate.h"

int game_state_init(struct GameState* game_state) {
  vector_init(&game_state->entities, sizeof(struct Entity));
  component_registry_init(&game_state->position_registry, sizeof(struct Position));
  component_registry_init(&game_state->dimensions_registry, sizeof(struct Dimensions));
  component_registry_init(&game_state->scenery_registry, sizeof(struct Scenery));

  game_state->game_stage = calloc(1, sizeof(struct GameStage));
  game_stage_init(game_state->game_stage);

  return 0;
}

void game_state_delete(struct GameState* game_state) {
  game_stage_delete(game_state->game_stage);
  free(game_state->game_stage);

  // Delete ecs
  vector_delete(&game_state->entities);
  component_registry_delete(&game_state->position_registry);
  component_registry_delete(&game_state->dimensions_registry);
  component_registry_delete(&game_state->scenery_registry);
}

void game_state_update(struct GameState* game_state, double delta_time) {}