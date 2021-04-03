#pragma once
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <mana/core/memoryallocator.h>
//
#include <mana/audio/audiomanager.h>
#include <mana/graphics/utilities/texturecache.h>
#include <mana/mana.h>
#include <xmlparser.h>

struct ResourceManager {
  // TODO: Move to someplace more abstract this is way too specfic
  struct AudioManager audio_manager;

  struct TextureCache texture_cache;
};

int resource_manager_init(struct ResourceManager* resource_manager, struct GPUAPI* gpu_api);
void resource_manager_delete(struct ResourceManager* resource_manager, struct GPUAPI* gpu_api);

#endif  // RESOURCE_MANAGER_H
