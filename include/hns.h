#pragma once
#ifndef HNS_H
#define HNS_H

#include <mana/core/memoryallocator.h>
//
#include <mana/mana.h>

#include "state/game.h"

enum STATE_TYPE {
  STATE_GAME = 0
};

struct State {
  void* state;
  void (*state_func)(void*, struct Mana*, double);
  enum STATE_TYPE state_type;
};

struct HNS {
  struct Mana mana;
  struct Window window;

  struct State state;
};

enum HNS_STATUS {
  HNS_SUCCESS = 0,
  HNS_MANA_ERROR,
  HNS_LAST_ERROR
};

int hns_init(struct HNS* hns);
void hns_delete(struct HNS* hns);
void hns_start(struct HNS* hns);

static inline void hns_change_state(struct HNS* hns, enum STATE_TYPE state_type) {
  hns->state.state_type = state_type;
  switch (state_type) {
    case (STATE_GAME):
      hns->state.state = calloc(1, sizeof(struct Game));
      hns->state.state_func = (void (*)(void*, struct Mana*, double))game_update;
      game_init(hns->state.state, &hns->mana, &hns->window);
      break;
  }
}

#endif  // HNS_H
