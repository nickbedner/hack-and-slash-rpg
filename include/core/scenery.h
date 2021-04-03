#pragma once
#ifndef SCENERY_H
#define SCENERY_H

#include <mana/core/memoryallocator.h>
//
#include <cstorage/map.h>
#include <cstorage/vector.h>
#include <ubermath/ubermath.h>

#include "core/entity.h"

struct Scenery {
  char* texture_path;
  int repeat_factor;
  float offset;
};

#endif  // SCENERY_H