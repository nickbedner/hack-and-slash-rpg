#pragma once
#ifndef ENTITY_H
#define ENTITY_H

#include <mana/core/memoryallocator.h>
//
#include <cstorage/map.h>
#include <cstorage/vector.h>
#include <ubermath/ubermath.h>

/*struct EntityUniqueComponents {
  void* entity_data;
  void (*delete_func)(void*, void*);
  void (*update_func)(void*, void*, float);
  void (*render_func)(void*, void*);
  void (*recreate_func)(void*, void*);
};*/

struct Entity {
  char entity_id[37];
};

static inline int entity_init(struct Entity* entity) {
  // TODO: Generate proper uuid
  for (int char_num = 0; char_num < 36; char_num++) {
    entity->entity_id[char_num] = (rand() % 10) + '0';
    //entity->entity_id[char_num] = 'a';
  }
  entity->entity_id[36] = '\0';

  return 0;
}

//struct EntityUpdateData {
//  void* entity_handle;
//  void* game_handle;
//  float delta_time;
//};
//
//static inline void entity_update_job(void* data) {
//  struct EntityUpdateData* update_data = data;
//  struct Entity* entity = update_data->entity_handle;
//  (*entity->update_func)(entity->entity_data, update_data->game_handle, update_data->delta_time);
//}

struct ComponentRegistry {
  struct Map registry;
  size_t memory_size;
};

static inline int component_registry_init(struct ComponentRegistry* component_registry, size_t memory_size) {
  map_init(&component_registry->registry, memory_size);
  component_registry->memory_size = memory_size;

  return 0;
}

static inline void component_registry_delete(struct ComponentRegistry* component_registry) {
  map_delete(&component_registry->registry);
}

static inline void component_registry_set(struct ComponentRegistry* component_registry, struct Entity* entity, void* new_component) {
  // When data oriented set vector then map should be references
  map_set(&component_registry->registry, entity->entity_id, new_component);
}

static inline void* component_registry_get(struct ComponentRegistry* component_registry, char* entity_id) {
  return map_get(&component_registry->registry, entity_id);
}

static inline void component_registry_remove(struct ComponentRegistry* component_registry, char* entity_id) {
  map_remove(&component_registry->registry, entity_id);
}

#endif  // ENTITY_H