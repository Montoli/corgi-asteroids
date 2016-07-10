// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <assert.h>
#include "corgi/system_id_lookup.h"
#include "corgi/entity_manager.h"
#include "corgi/version.h"

namespace corgi {

// The reference to the version string is important because it ensures that
// the constant won't be stripped out by the compiler.
EntityManager::EntityManager()
    : entity_factory_(nullptr),
      version_(&Version()) {}

// Allocates a new entity and returns it
EntityRef EntityManager::AllocateNewEntity() {
  EntityRef result = EntityRef(entities_.GetNewElement(kAddToBack));
  // Indexes are guaranteed to be unique and stable in vector pools.
  // May need to revisit this if we implement defragging though.
  result->set_entity_id(static_cast<EntityIdType>(result.index()));
  return result;
}

// Note: This function doesn't actually delete the entity immediately -
// it just marks it for deletion, and it gets cleaned out at the end of the
// next AdvanceFrame.
void EntityManager::DeleteEntity(EntityRef entity) {
  if (entity->marked_for_deletion()) {
    // already marked for deletion.
    return;
  }
  entity->set_marked_for_deletion(true);
  entities_to_delete_.push_back(entity);
}

// This deletes the entity instantly.  You should generally use the regular
// DeleteEntity unless you have a particuarly good reason to need it instantly.
void EntityManager::DeleteEntityImmediately(EntityRef entity) {
  RemoveAllSystems(entity);
  entities_.FreeElement(entity);
}

void EntityManager::DeleteMarkedEntities() {
  for (size_t i = 0; i < entities_to_delete_.size(); i++) {
    EntityRef entity = entities_to_delete_[i];
    RemoveAllSystems(entity);
    entities_.FreeElement(entity);
  }
  entities_to_delete_.resize(0);
}

void EntityManager::RemoveAllSystems(EntityRef entity) {
  for (SystemId i = 0; i < systems_.size(); i++) {
    if (systems_[i] != nullptr && systems_[i]->HasDataForEntity(entity)) {
      systems_[i]->RemoveEntity(entity);
    }
  }
}

void EntityManager::AddEntityToSystem(EntityRef entity,
                                         SystemId system_id) {
  SystemInterface* system = GetSystem(system_id);
  assert(system != nullptr);
  system->AddEntityGenerically(entity);
}

void EntityManager::RegisterSystemHelper(SystemInterface* new_system,
                                            SystemId id) {
  // Make sure this ID isn't already associated with a system.
  systems_.push_back(new_system);
  assert(new_system == systems_[id]);
  new_system->SetEntityManager(this);
  new_system->SetSystemIdOnDataType(id);
  new_system->Init();
}

void EntityManager::FinalizeSystemList() {
  for (size_t i = 0; i < systems_.size(); i++) {
    if (systems_[i]) systems_[i]->DeclareDependencies();
  }
  is_system_list_final_ = true;
  //todo: validate dependencies, make sure none are circular, etc.
}


void* EntityManager::GetSystemDataAsVoid(EntityRef entity,
                                            SystemId system_id) {
  return systems_[system_id]
             ? systems_[system_id]->GetSystemDataAsVoid(entity)
             : nullptr;
}

const void* EntityManager::GetSystemDataAsVoid(
    EntityRef entity, SystemId system_id) const {
  return systems_[system_id]
             ? systems_[system_id]->GetSystemDataAsVoid(entity)
             : nullptr;
}

void EntityManager::UpdateSystems(WorldTime delta_time) {
  // Assert if you haven't finalized the system list.
  assert(is_system_list_final_);
  // Update all the registered systems.
  for (size_t i = 0; i < systems_.size(); i++) {
    if (systems_[i]) systems_[i]->UpdateAllEntities(delta_time);
  }
  DeleteMarkedEntities();
}

void EntityManager::Clear() {
  for (size_t i = 0; i < systems_.size(); i++) {
    if (systems_[i]) {
      systems_[i]->ClearComponentData();
      systems_[i]->Cleanup();
    }
  }
  systems_.clear();
  entities_.Clear();
}

EntityRef EntityManager::CreateEntityFromData(const void* data) {
  assert(entity_factory_ != nullptr);
  return entity_factory_->CreateEntityFromData(data, this);
}

}  // corgi
