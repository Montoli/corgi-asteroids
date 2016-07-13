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

#ifndef CORGI_COMPONENT_INTERFACE_H_
#define CORGI_COMPONENT_INTERFACE_H_

#include <stdint.h>
#include <functional>
#include <memory>
#include "corgi/entity.h"
#include "corgi/entity_common.h"
#include "corgi/entity_manager.h"
#include "corgi/vector_pool.h"

namespace corgi {

class EntityManager;

/// @file
/// @addtogroup corgi_component
/// @{
///
/// @typedef EntityRef
///
/// @brief A typedef that should be used as the primary way to reference
/// an Entity.
///
/// An EntityRef can be treated like a pointer to an Entity. In most cases,
/// it functions interchangeably with a normal pointer, but it also contains
/// extra functionality for determining if the EntityRef is invalid. For
/// instance, if the Entity that the EntityRef points to is deallocated, the
/// EntityRef will no longer be valid (even if new data exists in the same
/// memory location previously held by the deallocated Entity).
///
/// EntityRefs are typically passed by reference or const reference. If an
/// EntityRef is const, then you can only access the underlying Entity data
/// in a read-only manner.
typedef VectorPool<corgi::Entity>::VectorPoolReference EntityRef;

/// @class SystemInterface
///
/// @brief An interface that provides basic System functionality.
/// All Components will inherit from this class. It provides the minimum set
/// of things that are uniform across all Components (without needing to know
/// the specific type of System that it is).
class SystemInterface {
public:
	/// @typedef RawDataUniquePtr
	///
	/// @brief A pointer type for exported raw data.
	typedef std::unique_ptr<uint8_t, std::function<void(uint8_t*)>>
		RawDataUniquePtr;

	/// @brief A destructor for the System interface.
	virtual ~SystemInterface() {}

	/// @brief Add an Entity to the System.
	///
	/// @note Usually you will want to use System::AddEntity, since that
	/// returns a pointer to the data assigned to the System.
	///
	/// @param[in,out] entity An EntityRef reference to the Entity being added to
	/// this System.
	virtual void AddEntityGenerically(EntityRef& entity) = 0;

	/// @brief Remove an Entity from the System's list.
	///
	/// @param[in] entity An EntityRef reference to the Entity being remove
	/// from this System.
	virtual void RemoveEntity(EntityRef& entity) = 0;

	/// @brief Update all Entities that contain this System.
	///
	/// @param[in] delta_time A WorldTime corresponding to the
	/// delta time for this frame.
	virtual void UpdateAllEntities(WorldTime delta_time) = 0;

	/// @brief Returns true if this component has data associated with the
	/// entity provided.
	virtual bool HasDataForEntity(const EntityRef&) = 0;

	/// @brief Clears all System data, effectively disassociating this
	/// System from any Entities.
	virtual void ClearComponentData() = 0;

	/// @brief Gets the data for a given Entity as a void pointer.
	///
	/// @note When using GetSystemDataAsVoid, the calling function is expected
	/// to know how to handle the data (since it is returned as a void pointer).
	///
	/// @warning This pointer is NOT stable in memory. Calls to
	/// AddEntityGenerically may force the storage class to resize,
	/// shuffling around the location of this data.
	///
	/// @return Returns the Entity's data as a void pointer, or returns a nullptr
	/// if the data does not exist.
	virtual void* GetSystemDataAsVoid(const EntityRef&) = 0;

	/// @brief Gets the data for a given ntity as a const void pointer.
	///
	/// @note When using GetSystemDataAsVoid, the calling function is expected
	/// to know how to handle the data (since it is returned as a const
	/// void pointer).
	///
	/// @warning This pointer is NOT stable in memory. Calls to AddEntity and
	/// AddEntityGenerically may force the storage class to resize,
	/// shuffling around the location of this data.
	///
	/// @return Returns the Entity's data as a const void pointer, or returns a
	/// nullptr if the data does not exist.
	virtual const void* GetSystemDataAsVoid(const EntityRef&) const = 0;

	/// @brief This function is called after the System is added to the
	/// EntityManager. (i.e. This typically happens once, at the beginning
	/// of the game before any Entities are added.)
	virtual void Init() = 0;

	/// @brief Invoked by EntityManager::FinalizeSystemList, once all systems
	// have been registered.  This is the place for systems to declare any
	/// dependencies they have on other systems, if any. (Via System::DependOn)
	virtual void DeclareDependencies() = 0;

  /// @brief Declare a specific dependency on another system.
  virtual void DependOn(SystemId system_id,
                        SystemOrderDependencyType order_dependency,
                        SystemAccessDependencyType access_dependency) = 0;

  /// @brief Called by the EntityManager every time an Entity is added to this
  /// System.
  ///
  /// @param[in] entity An EntityRef pointing to an Entity that is being added
  /// to this System and may need initialized.
  virtual void InitEntity(EntityRef& entity) = 0;

  /// @brief Creates and populates an Entity from raw data. Components that want
  /// to be able to be constructed via the EntityFactory need to implement this.
  ///
  /// @param[in,out] entity An EntityRef that points to an Entity that is being
  /// added from the raw data.
  /// @param[in] data A void pointer to the raw data.
  virtual void AddFromRawData(EntityRef& entity, const void* data) = 0;

  /// @brief Serializes a System's data for a specific Entity.
  ///
  /// If you do not support this functionality, this function
  /// should return a nullptr.
  ///
  /// @param[in] entity An EntityRef reference to an Entity whose raw data
  /// should be returned.
  ///
  /// @return Returns a RawDataUniquePtr to the raw data.
  virtual RawDataUniquePtr ExportRawData(const EntityRef& entity) const = 0;

  /// @brief Called just before removal from the EntityManager. (i.e.
  /// Usually when the game/state is over and everything is shutting
  /// down.)
  virtual void Cleanup() = 0;

  /// @brief Called when the Entity is being removed from the System.
  /// Components should implement this if they need to perform any cleanup
  /// on the Entity data.
  ///
  /// @param[in] entity An EntityRef reference to the Entity that is being
  /// removed and may need to be cleaned up.
  virtual void CleanupEntity(EntityRef& entity) = 0;

  /// @brief Set the EntityManager for this System. Usually this
  /// is assigned by the EntityManager itself.
  ///
  /// @note The EntityManager is used as the main point of contact
  /// for Components that need to talk to other things.
  ///
  /// @param[in] entity_manager A pointer to an EntityManager to associate
  /// with this System.
  virtual void SetEntityManager(EntityManager* entity_manager) = 0;

  /// @brief Sets the System ID for the data type.
  ///
  /// @note This is normally only called once by the EntityManager.
  ///
  /// @param[in] id The System ID to set for the data type.
  virtual void SetSystemIdOnDataType(SystemId id) = 0;

  /// @brief Determines whether this system is safe to farm out to a
  /// separate thread for faster updates.  Disabled by default.
  ///
  /// @return Returns true if this system can be safely executed from
  /// a different (non-main) thread.
  virtual bool IsThreadSafe() = 0;

  /// @brief Used to mark a system as being safe to execute from a
  /// separate thread.  By default, all systems are unsafe, so they
  /// will simply execute in dependency-order on the main thread.  Any
  /// systems marked as thread-safe will 
  ///
  /// @param[in] is_thread_safe a boolean specifying the thread-safety
  /// of this system.  True means that it is thread-safe.
  virtual void SetIsThreadSafe(bool is_thread_safe) = 0;
};
/// @}

}  // corgi

#endif  // CORGI_COMPONENT_INTERFACE_H_
