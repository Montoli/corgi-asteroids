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

#define DEFAULT_MAX_THREADS 3

// The reference to the version string is important because it ensures that
// the constant won't be stripped out by the compiler.
EntityManager::EntityManager()
    : entity_factory_(nullptr),
      version_(&Version()),
			exit_worker_threads_(false),
			bookkeeping_mutex_(SDL_CreateMutex()),
			worker_thread_mutex_(SDL_CreateMutex()),
			worker_thread_cond_(SDL_CreateCond()),
			max_worker_threads_(DEFAULT_MAX_THREADS - 1) {}

EntityManager::~EntityManager() {
	SDL_DestroyMutex(bookkeeping_mutex_);
	SDL_DestroyMutex(worker_thread_mutex_);
	SDL_DestroyCond(worker_thread_cond_);
}

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


	for (int i = 0; i < max_worker_threads_; i++) {
		SDL_CreateThread(EntityManager::EntityManagerWorkerThread,
			"WorkerThread", this);

	}
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

	// save off the delta time, so that worker threads can see it.
	delta_time_ = delta_time;

	// The current-read/write status should be clear, but we'll double
	// check because paranoia.
	for (size_t i = 0; i < systems_.size(); i++) {
		SystemId system_id = static_cast<SystemId>(i);
		assert(systems_being_read_from_[system_id] == 0);
		assert(systems_being_written_to_[system_id] == 0);
	}
	unupdated_systems_.clear();
	currently_updating_systems_.clear();
	updated_systems_.clear();

	for (size_t i = 0; i < systems_.size(); i++) {
		unupdated_systems_.insert(systems_[static_cast<SystemId>(i)]->GetSystemId());
	}

	WakeWorkerThreads();
	while (!IsSystemUpdateComplete()) {
		SystemId system_id = ClaimSystemToUpdate(true);
		// Lock the mutex BEFORE we do our comparisons
		// to make sure no one broadcasts after we commit
		// to CondWaiting, but before we actually CondWait...
		SDL_LockMutex(worker_thread_mutex_);
		if (system_id == kInvalidEntityId && !IsSystemUpdateComplete()) {
			SDL_CondWait(worker_thread_cond_,
				worker_thread_mutex_);
			SDL_UnlockMutex(worker_thread_mutex_);
		} else {
			SDL_UnlockMutex(worker_thread_mutex_);

			SystemInterface* system = GetSystem(system_id);
			system->UpdateAllEntities(delta_time_);
			MarkSystemAsUpdated(system_id);
			WakeWorkerThreads();
		}
	}

	// Sanity checking - we should not be reading/writing from anything
	// when we are done with updates.
	SDL_LockMutex(bookkeeping_mutex_);
	for (size_t i = 0; i < systems_.size(); i++) {
		SystemId system_id = static_cast<SystemId>(i);
		if (systems_being_read_from_[system_id] != 0) {
			printf("System not finished: [%s]\n", GetSystem(system_id)->Name());
		}
		assert(systems_being_read_from_[system_id] == 0);
		assert(systems_being_written_to_[system_id] == 0);
	}
	SDL_UnlockMutex(bookkeeping_mutex_);

	DeleteMarkedEntities();
}

bool EntityManager::IsSystemUpdateComplete() {
	// Have to lock the bookkeeping mutex, so that it doesn't change 
	// while we're adding this.  (Which will lead to false positives...)
	SDL_LockMutex(bookkeeping_mutex_);
	bool result = !(unupdated_systems_.size() +
		currently_updating_systems_.size() > 0);
	SDL_UnlockMutex(bookkeeping_mutex_);
	return result;
}

void EntityManager::WakeWorkerThreads() {
	// tell every other worker thread (and the main game!) that
	// something completed.
	SDL_LockMutex(worker_thread_mutex_);
	SDL_CondBroadcast(worker_thread_cond_);
	SDL_UnlockMutex(worker_thread_mutex_);
}

/// @brief Searches through unupdated systems until it finds one that
/// is legal to begin updating.  (No unupdated dependencies, and no
/// read/write blocks.)
///
/// @param[in] is_for_main_thread If set to true, FindSystemToUpdate will
/// try to return systems that need to execute from the main thread, and
/// will not return systems that can't.
///
/// @return The system ID of the system to update.  Returns kInvalidSystem
/// if none could be found.
SystemId EntityManager::ClaimSystemToUpdate(bool is_for_main_thread) {
	SystemId result = kInvalidSystem;

	// Mutex begin!  (need to make sure this data doesn't change out from
	// under us while we're evaluating it.)
	SDL_LockMutex(bookkeeping_mutex_);

	// Todo:  Make this smarter, so that it favors getting things out of the
	// way earlier, if they have lots of dependencies.
	for (auto itr = unupdated_systems_.begin();
		itr != unupdated_systems_.end(); ++itr) {
		SystemId current_id = *itr;
		SystemInterface* current_system = GetSystem(current_id);
		assert(current_system);


		bool current_system_ok = true;

		// Check that all dependencies have been updated.
		for (auto dep = current_system->ExecuteDependencies()->begin();
			  dep != current_system->ExecuteDependencies()->end(); ++dep) {
			if (updated_systems_.find(*dep) == updated_systems_.end()) {
				// it has one or more dependencies that need to be
				// executed before it can execute.
				current_system_ok = false;
				break;
			}
		}
		if (!current_system_ok) continue;

		// Check that this system doesn't need access to anything that someone
		// else is writing.
		for (auto dep = current_system->AccessDependencies()->begin();
			dep != current_system->AccessDependencies()->end(); ++dep) {
			// If someone else is writing to a system we need (for read or write)
			// then this won't work. Keep looking.
			if (systems_being_written_to_[dep->first] != 0) {
				current_system_ok = false;
				break;
			}
			// If we need to write to a system, then we also need to make
			// sure that no one else is reading from it.
			if (dep->second == kReadWriteAccess &&
				  systems_being_read_from_[dep->first] != 0) {
				current_system_ok = false;
				break;
			}

		}
		if (!current_system_ok) continue;

		// If we've made it this far, we've found a good system.

		// Check to make sure that it matches main_thread_only.
		if (is_for_main_thread) {
			// If we're looking for something for the main thread,
			// favor things that are not thread-safe over things that are,
			// to try to save the thread-safe stuff for worker threads.
			if (current_system->IsThreadSafe()) {
				// we're going to remember this one, but it's not ideal,
				// since it's thread safe.  (So some worker thread could
				// handle it instead of the main thread, if we wanted.)
				// So we'll keep looking, just in case we find something
				// better.
				result = current_id;
				continue;
			} else {
				// This is not thread safe, so it's a great thing to have
				// the main thread work on next.  Select it!
				result = current_id;
				break;
			}
		} else {
			// If we're looking for something for a worker thread, then
			// just return the first thread-safe thing we can find.
			if (current_system->IsThreadSafe()) {
				// We have a thread-safe system, and they are asking for one.
				// Give it to them!
				result = current_id;
				break;
			} else {
				// Can't return something that's not threadsafe if they
				// are asking for something to run on a thread...
				continue;
			}
		}
	}


	if (result != kInvalidSystem)
		MarkSystemAsUpdating(result);
	// Mutex end!

	SDL_UnlockMutex(bookkeeping_mutex_);


	return result;
}

void EntityManager::MarkSystemAsUpdating(SystemId systemId) {
	//  NOTE:  We do NOT need to lock the mutex here, becuase
	// this function is only ever called from witin ClaimSystemToUpdate.
	//  ALSO NOTE:  Do not call this function from anywhere except
	// from within ClaimSystemToUpdate.
	// verify that this system has not been updated yet.
	assert(unupdated_systems_.find(systemId) != unupdated_systems_.end());

	// Move it to the list of systems being updated.
	unupdated_systems_.erase(systemId);
	currently_updating_systems_.insert(systemId);

	SystemInterface* system = GetSystem(systemId);

	// Mark its dependencies as in-use.
	for (auto dep = system->AccessDependencies()->begin();
		dep != system->AccessDependencies()->end(); ++dep) {
		switch (dep->second) {
		case kReadAccess:
			systems_being_read_from_[dep->first]++;
			break;
		case kReadWriteAccess:
			// You should never be writing to something that someone else
			// is already modifying.
			assert(systems_being_written_to_[dep->first] == 0);
			systems_being_written_to_[dep->first]++;
			break;
		default:
			// If you got here, something is very wrong.
			assert(false);
		}
	}
}

void EntityManager::MarkSystemAsUpdated(SystemId systemId) {
	SDL_LockMutex(bookkeeping_mutex_);

	assert(currently_updating_systems_.find(systemId) !=
		currently_updating_systems_.end());

	// Move it to the list of systems being updated.
	currently_updating_systems_.erase(systemId);
	updated_systems_.insert(systemId);

	SystemInterface* system = GetSystem(systemId);

	// Mark its dependencies as in-use.
	for (auto dep = system->AccessDependencies()->begin();
		dep != system->AccessDependencies()->end(); ++dep) {
		switch (dep->second) {
		case kReadAccess:
			systems_being_read_from_[dep->first]--;
			// If this goes below zero, our bookkeeping is probably off.
			assert(systems_being_read_from_[dep->first] >= 0);
			break;
		case kReadWriteAccess:
			// You should never be writing to something that someone else
			// is already modifying.
			systems_being_written_to_[dep->first]--;
			assert(systems_being_written_to_[dep->first] >= 0);
			break;
		default:
			// If you got here, something is very wrong.
			assert(false);
		}
	}

	SDL_UnlockMutex(bookkeeping_mutex_);
}


// This is what one of our worker threads looks like....
int EntityManager::EntityManagerWorkerThread(void * data) {
	EntityManager* entity_manager = static_cast<EntityManager*>(data);
	while (!entity_manager->exit_worker_threads_) {
		SystemId system_id = entity_manager->ClaimSystemToUpdate(false);
		if (system_id == kInvalidEntityId) {
			SDL_LockMutex(entity_manager->worker_thread_mutex_);
			SDL_CondWait(entity_manager->worker_thread_cond_,
				  entity_manager->worker_thread_mutex_);
			SDL_UnlockMutex(entity_manager->worker_thread_mutex_);
		} else {
			SystemInterface* system = entity_manager->GetSystem(system_id);
			system->UpdateAllEntities(entity_manager->delta_time_);
			entity_manager->MarkSystemAsUpdated(system_id);

			// tell every other worker thread (and the main game!) that
			// something completed.
			entity_manager->WakeWorkerThreads();
		}
	}
	return 0;
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
