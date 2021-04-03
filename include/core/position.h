#pragma once
#ifndef POSITION_H
#define POSITION_H

#include <mana/core/memoryallocator.h>
//
#include <cstorage/map.h>
#include <cstorage/vector.h>
#include <ubermath/ubermath.h>

#include "core/entity.h"

struct Position {
  float x;
  float y;
  float z;
};

//struct PositionComponentPsuedoDO {
//  struct Vector x;
//  struct Vector y;
//  struct Vector z;
//};

#endif  // POSITION_H