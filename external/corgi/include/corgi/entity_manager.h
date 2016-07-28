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

#ifndef CORGI_ENTITY_MANAGER_H_
#define CORGI_ENTITY_MANAGER_H_

#include <SDL.h>
#include <unordered_set>
#include <map>
#include "corgi/system_id_lookup.h"
#include "corgi/system_interface.h"
#include "corgi/entity_common.h"
#include "corgi/version.h"
#include <cassert>

namespace corgi {

/// @file
/// @addtogroup corgi_entity_manager
/// @{
///
/// @typedef Entity
///
/// @brief This should be used as the primary way to reference an Entity.
///
/// An Entity can be treated like a pointer to an Entity. In most cases,
/// it functions interchangeably with a normal pointer, but it also contains
/// extra functionality for determining if the Entity is invalid. For
/// instance, if the Entity that the Entity points to is deallocated, the
/// Entity will no longer be valid (even if new data exists in the same
/// memory location previously held by the deallocated Entity).
///
/// EntityRefs are typically passed by reference or const reference. If an
/// Entity is const, then you can only access the underlying Entity data
/// in a read-only manner.
//typedef VectorPool<Entity>::VectorPoolReference Entity;

class EntityFactoryInterface;
class SystemInterface;
class EntityManager;

/// @class EntityManager
/// @brief The EntityManager is the code that manages all
/// Entities and Components in the game. Normally the game will
/// instantiate EntityManager, and then use it to create and control
/// all of its Entities.
///
/// The main uses for this class are:
///  * Creating/destroying new Entities.
///  * Keeping track of all Components.
///  * Spawning and populating new Components from data.
///  * Updating Entities/Components.  (Usually done once per frame.)
class EntityManager {
 public:
  /// @brief Constructor for the EntityManager.
  EntityManager();
	 
	/// @brief Destructor for the EntitnyManager.
	~EntityManager();

  /// @brief Returns the version of the Corgi entity library.
  const CorgiVersion* GetCorgiVersion() { return version_; }

  /// @typedef EntityStorageContainer
  ///
  /// @brief This is used to track all Entities stored by the EntityManager.
  typedef std::unordered_set<Entity> EntityStorageContainer;

  /// @brief Helper function for marshalling data from a System.
  ///
  /// @tparam T The data type of the System data to be returned.
  ///
  /// @param[in] entity The Entity associated with the desired data.
  ///
  /// @return Returns a pointer to the System data, or returns
  /// a nullptr if no such data exists.
  template <typename T>
  T* GetComponentData(const Entity entity) {
    return static_cast<T*>(
        GetSystemDataAsVoid(entity, SystemIdLookup<T>::system_id));
  }

  /// @brief A helper function for marshalling data from a System.
  ///
  /// @tparam T The data type of the System data to be returned.
  ///
  /// @param[in] entity The Entity associated with the desired data.
  ///
  /// @return Returns a const pointer to the System data, or returns
  /// a nullptr if no such data exists.
  template <typename T>
  const T* GetComponentData(const Entity entity) const {
    return static_cast<const T*>(
        GetSystemDataAsVoid(entity, SystemIdLookup<T>::system_id));
  }

  /// @brief A helper function for getting a particular System, given
  /// the System's data type.
  ///
  /// @note Asserts if the System does not exist.
  ///
  /// @tparam T The data type of the System to be returned.
  ///
  /// @return Returns a pointer to the System given its data type.
  template <typename T>
  T* GetSystem() {
    SystemId id =
        static_cast<SystemId>(SystemIdLookup<T>::system_id);
    assert(id != kInvalidSystem);
    assert(id < systems_.size());
    return static_cast<T*>(systems_[id]);
  }

  /// @brief A helper function for getting a particular System, given
  /// the System's data type.
  ///
  /// @note Asserts if the System does not exist.
  ///
  /// @tparam T The data type of the System to be returned.
  ///
  /// @return Returns a const pointer to the System given its datatype.
  template <typename T>
  const T* GetSystem() const {
    SystemId id =
        static_cast<SystemId>(SystemIdLookup<T>::system_id);
    assert(id != kInvalidSystem);
    assert(id < systems_.size());
    return static_cast<const T*>(systems_[id]);
  }

  /// @brief A helper function for adding a System to an Entity, given
  /// its data type.
  ///
  /// @note Asserts if the System does not exist.
  ///
  /// @tparam T The data type of the System that should have the Entity
  /// added to it.
  ///
  /// @param[in] entity An Entity that points to the Entity that is being
  /// added to the System.
  template <typename T>
  void AddComponent(const Entity entity) {
    SystemId id =
        static_cast<SystemId>(SystemIdLookup<T>::system_id);
    assert(id != kInvalidSystem);
    assert(id < systems_.size());
    AddComponent(entity, id);
  }

  /// @brief A helper function for getting a particular System, given the
  /// component ID.
  ///
  /// @param[in] id The component ID for the desired System.
  ///
  /// @note Asserts if the id is less than kMaxComponentCount.
  ///
  /// @return Returns a pointer to the System at the given id, which
  /// inherits from the SystemInterface.
  inline SystemInterface* GetSystem(SystemId id) {
    assert(id < systems_.size());
    return systems_[id];
  }

  /// @brief A helper function for getting a particular System, given a
  /// component ID.
  ///
  /// @param[in] id The component ID for the desired System.
  ///
  /// @note Asserts if the id is less than kMaxComponentCount.
  ///
  /// @return Returns a const pointer to the System at the given id,
  /// which inherits from the SystemInterface.
  inline const SystemInterface* GetSystem(SystemId id) const {
    assert(id < systems_.size());
    return systems_[id];
  }

  /// @brief Returns the number of components that have been registered
  /// with the entity manager.
  ///
  /// @return The total number of components that are currently registered
  /// with the entity manager.
  inline size_t ComponentCount() const { return systems_.size(); }

  /// @brief A helper function to get the component ID for a given System.
  ///
  /// @tparam T The data type of the System whose ID should be returned.
  ///
  /// @return Returns the component ID for a given System.
  template <typename T>
  SystemId GetComponentId() {
    return SystemIdLookup<T>::system_id;
  }

  /// @brief Allocates a new Entity (that is registered with no Components).
  ///
  /// @return Returns an Entity that points to the new Entity.
	Entity AllocateNewEntity();

  /// @brief Deletes an Entity by removing it from the EntityManager's list and
  /// clearing any System data associated with it.
  ///
  /// @note Deletion is deferred until the end of the frame. If you want to
  /// delete something instantly, use DeleteEntityImmediately.
  ///
  /// @param[in] entity An Entity that points to the Entity that will be
  /// deleted at the end of the frame.
  void DeleteEntity(Entity entity);

  /// @brief Instantly deletes an Entity.
  ///
  /// @note In general, you should use DeleteEntity (which defers deletion
  /// until the end of the update cycle) unless you have a very good reason
  /// for doing so.
  ///
  /// @param[in] entity An Entity that points to the Entity that will be
  /// immediately deleted.
  void DeleteEntityImmediately(Entity entity);

  /// @brief Registers a new System with the EntityManager.
  ///
  /// @tparam T The data type of the System that is being registered with the
  /// EntityManager.
  ///
  /// @param[in] new_system A System to be registered with to the
  /// EntityManager.
  ///
  /// @note The new_system must inherit from the SystemInterface.
  ///
  /// @return Returns the system ID for the new System.
  template <typename T>
  SystemId RegisterSystem(T* new_system) {
    static_assert(std::is_base_of<SystemInterface, T>::value,
                  "'new_system' must inherit from SystemInterface");
    SystemId system_id = static_cast<SystemId>(systems_.size());
    SystemIdLookup<T>::system_id = system_id;
    RegisterSystemHelper(new_system, system_id);
    return system_id;
  }

  /// @brief Finalizes the list of systems and the dependency graph.
  ///
  /// @note Further calls to Register System will fail if called after
  /// FinalizeSystemList
  void FinalizeSystemList();

  /// @brief Removes all Components for an Entity, destroying any data
  /// associated with it.
  ///
  /// @note Normally called by EntityManager prior to deleting an Entity.
  ///
  /// @param[in] entity An Entity that points to the Entity whose Components
  /// should be removed.
  void RemoveAllSystems(Entity entity);

  /// @brief Iterates through all the registered Components and causes them to
  /// update.
  ///
  /// @param[in] delta_time A WorldTime that represents the timestep since
  /// the last update.
  void UpdateSystems(WorldTime delta_time);

  /// @brief Clears all data from all Components, empties the list
  /// of Components themselves, and then empties the list of Entities.
  /// This basically resets the EntityManager into its original state.
  void Clear();

  /// @brief Returns an iterator to the beginning of the active Entities.
  /// This is suitable for iterating over every active Entity.
  ///
  /// @return Returns a std::iterator to the first active Entity.
  EntityStorageContainer::iterator begin() { return entities_.begin(); }

  /// @brief Returns an iterator to the last of the active Entities.
  ///
  /// @return Returns a std::iterator to the last active Entity.
  EntityStorageContainer::iterator end() { return entities_.end(); }

  /// @brief Registers an instance of the EntityFactoryInterface as the
  /// factory object to be used when Entities are created from arbitrary
  /// data. The EntityFactory is responsible for parsing arbitrary data and
  /// correctly transforming it into an Entity.
  ///
  /// @note This should be set before attempting to load Entities from data
  /// files.
  ///
  /// @param[in] entity_factory A pointer to a class that inherits from the
  /// EntityFactoryInterface, which will be registered with the
  /// EntityManager.
  void set_entity_factory(EntityFactoryInterface* entity_factory) {
    entity_factory_ = entity_factory;
  }

  /// @brief Creates an Entity from arbitrary data. This is normally invoked
  /// only by classes that inherit from EntityFactoryInterface.
  ///
  /// Any class implementing CreateEntityFromData should establish the input
  /// format for the 'data' and a way to parse that input 'data' into an Entity.
  ///
  /// @param[in] data A void pointer to the data to be used to create the
  /// Entity.
  ///
  /// @return Returns an Entity that points to the newly created Entity.
  Entity CreateEntityFromData(const void* data);

  /// @brief Registers an Entity with a System. This causes the System
  /// to allocate data for the Entity and includes the Entity in that
  /// System's update routines.
  ///
  /// @param[in] entity An Entity that points to the Entity that should be
  /// registered with the System.
  /// @param[in] system_id The component ID of the System that should
  /// be registered with the Entity.
  void AddComponent(Entity entity, SystemId system_id);

  /// @brief Deletes all the Entities that are marked for deletion.
  ///
  /// @warning Do NOT call this function during any form of Entity update!
  void DeleteMarkedEntities();

	/// @brief Checks if an entity is still valid.  A valid entity is
	/// defined as one that is still active, and has not been deleted.
	/// Note that entities that have been marked for deletion are still
	/// considered valid until they are actually deleted.
	///
	/// @param[in] entity An entity to check for validity.
	///
	/// @return Returns True if the entity is valid, false otherwise.
	bool IsEntityValid(Entity entity);

	/// @brief Checks if an entity is marked for deletion.  Marked entities
	/// will be removed at the end of the next update.
	///
	/// @param[in] entity An entity to check.
	///
	/// @return Returns True if the entity is marked for deletion,
	/// false otherwise.
	bool IsEntityMarkedForDeletion(Entity entity);

  /// @brief Boolean that tracks whether the list of systems has been finalized.
  /// (via FinalizeSystemList)  Once FinalizeSystemList has been called, entities are
  /// no longer allowed to change their dependencies.
  bool is_system_list_final() { return is_system_list_final_; }

	/// @brief Sets the max number of worker threads.  Must be called
	/// before FinalizeSystemLists or else it is ignored.
	void set_max_worker_threads(int max_worker_threads) {
		max_worker_threads_ = max_worker_threads;
	}

 private:
  /// @brief Handles the majority of the work for registering a System (
  /// aside from some of the template stuff). In particular, it verifies that
  /// the request ID is not already in use, puts a pointer to the new System
  /// in our systems_ array, sets the starting variables, and performs the
  /// initialization on the System.
  ///
  /// @param[in] new_system A pointer to a class that inherits from the
  /// SystemInterface, which will be registered with the system_id.
  /// @param[in] system_id The component ID to register with the
  /// new_system.
  void RegisterSystemHelper(SystemInterface* new_component,
                               SystemId system_id);

  /// @brief Given a component ID and an Entity, returns all data associated
  /// with that Entity from the given System.
  ///
  /// @note This function will assert if the inputs are not valid. (e.g. The
  /// Entity is no longer active, or the System is invalid.)
  ///
  /// @param[in] entity The Entity associated with the desired data.
  /// @param[in] system_id The component ID associated with the desired
  /// data.
  ///
  /// @return Returns the data as a void pointer.
  ///
  /// @note The caller is expected to know how to interpret the data, since it
  /// is returned as a void pointer. Typically a System is registered with a
  /// struct (or class) of System data, however this function returns the
  /// data as a void pointer, rather than a specific System data type.
  void* GetSystemDataAsVoid(Entity entity, SystemId system_id);

  /// @brief Given a component ID and an Entity, returns all data associated
  /// with that Entity from the given System.
  ///
  /// @note This function will assert if the inputs are not valid. (e.g. The
  /// Entity is no longer active, or the System is invalid.)
  ///
  /// @param[in] entity The Entity associated with the desired data.
  /// @param[in] system_id The component ID associated with the desired
  /// data.
  ///
  /// @return Returns the data as a void pointer.
  ///
  /// @note The caller is expected to know how to interpret the data, since it
  /// is returned as a void pointer. Typically a System is registered with a
  /// struct (or class) of System data, however this function returns the
  /// data as a void pointer, rather than a specific System data type.
  const void* GetSystemDataAsVoid(Entity entity,
                                     SystemId system_id) const;

  /// @brief Searches through unupdated systems until it finds one that
  /// is legal to begin updating.  (No unupdated dependencies, and no
  /// read/write blocks.)
  ///
  /// @param[in] main_thread_only Specifies if you only want systems that can
  /// be run from the main thread.  If set to true, FindSystemToUpdate will
  /// only return systems that need to execute from the main thread.
  ///
  /// @return The system ID of the system to update.  Returns kInvalidSystem
  /// if none could be found.
  SystemId ClaimSystemToUpdate(bool main_thread_only);

	// TODO(ccornell): write comments for these:
	void MarkSystemAsUpdating(SystemId system);
	void MarkSystemAsUpdated(SystemId system);

	void WakeWorkerThreads();

	static int EntityManagerWorkerThread(void*);

	// Utility function for checking if we've updated everything yet.
	bool IsSystemUpdateComplete();


  /// @var entities_
  ///
  /// @brief Storage for all the Entities currently tracked by the
  /// EntityManager.
	EntityStorageContainer entities_;

  /// @var systems_
  ///
  /// @brief All the Components that are tracked by the system, and are
  /// ready to have Entities added to them.
  std::vector<SystemInterface*> systems_;

  // todo(ccornell): write comments for these:
  std::unordered_set<SystemId> unupdated_systems_;
  std::unordered_set<SystemId> updated_systems_;
  std::unordered_set<SystemId> currently_updating_systems_;
	std::map<SystemId, int> systems_being_written_to_;
	std::map<SystemId, int> systems_being_read_from_;

	// Thread stuff:
	bool exit_worker_threads_;
	SDL_mutex* bookkeeping_mutex_;
	SDL_mutex* worker_thread_mutex_;
	SDL_cond* worker_thread_cond_;
	WorldTime delta_time_;
	int max_worker_threads_;

  /// @var entities_to_delete_
  ///
  /// @brief A list of all the Entities that we plan to delete at the end of the
  /// frame.
  ///
  /// @note These entities are deleted by a call to DeleteMarkedEntities().
  /// DeleteMarkedEntities should NOT be called called during any form of
  /// Entity update.
	EntityStorageContainer entities_to_delete_;

  /// @var entity_factory_
  ///
  /// @brief An EntityFactory used for spawning new Entities from data.
  ///
  /// @note Provided by the calling program.
  EntityFactoryInterface* entity_factory_;

  /// @var is_system_list_final_
  ///
  /// @brief Boolean that tracks whether the list of systems has been finalized.
  /// (via FinalizeSystemList)  Once FinalizeSystemList has been called, entities are
  /// no longer allowed to change their dependencies.
  bool is_system_list_final_;

	EntityIdType next_entity_id_;

  // Current version of the Corgi Entity Library.
  const CorgiVersion* version_;
};

/// @class EntityFactoryInterface
///
/// @brief An interface for an Entity factory, which creates Entities
/// for a given EntityManager.
class EntityFactoryInterface {
 public:
  /// @brief A destructor of the entity factory interface.
  virtual ~EntityFactoryInterface() {}

  /// @brief Creates an Entity with a given EntityManager, registers it
  /// with all Components specified, and populates the System data.
  ///
  /// @param[in] data A void pointer to the data used to create
  /// the new Entity.
  /// @param[in] entity_manager A pointer to an EntityManager that
  /// should create the Entity.
  ///
  /// @return Returns an Entity pointing to the newly created
  /// Entity.
  virtual Entity CreateEntityFromData(const void* data,
                                         EntityManager* entity_manager) = 0;
};
/// @}

}  // corgi

#endif  // CORGI_ENTITY_MANAGER_H_
