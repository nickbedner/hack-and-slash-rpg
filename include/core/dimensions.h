#pragma once
#ifndef DIMENSIONS_H
#define DIMENSIONS_H

#include <mana/core/memoryallocator.h>
//
#include <cstorage/map.h>
#include <cstorage/vector.h>
#include <ubermath/ubermath.h>

#include "core/entity.h"

struct Dimensions {
  float width;
  float height;
  float scale;
  float direction;
};

#endif  // DIMENSIONS_H