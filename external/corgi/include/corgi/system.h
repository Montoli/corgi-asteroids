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

#ifndef CORGI_COMPONENT_H_
#define CORGI_COMPONENT_H_

#include <unordered_map>
#include "corgi/system_id_lookup.h"
#include "corgi/system_interface.h"
#include "corgi/entity.h"
#include "corgi/entity_common.h"
#include "corgi/entity_manager.h"
#include "corgi/vector_pool.h"

namespace corgi {

/// @file
/// @addtogroup corgi_component
/// @{
///
/// @class System
/// @brief A System is an object that encapsulates all data and logic
/// for Entities of a particular type.
///
/// @note On some of the API, EntityRef& parameters are non-const so that
/// the referenced Entity can be changed. The EntityRef itself is never
/// modified.
///
/// @tparam T The structure of data that needs to be associated with each
/// Entity.
template <typename T>
class System : public SystemInterface {
 public:
  /// @struct ComponentData
  /// @brief A structure of data that is associated with each Entity.
  ///
  /// It contains the template struct, as well as a pointer back to the
  /// Entity that owns this data.
  struct ComponentData {
    /// @brief The default constructor for an empty ComponentData.
    ComponentData() {}

    /// @var entity
    ///
    /// @brief The Entity associated with this data.
    EntityRef entity;

    /// @var data
    ///
    /// @brief The data to associate with the Entity.
    T data;

    /// @brief Construct a new ComponentData from an existing ComponentData.
    ///
    /// @param[in] src An existing ComponentData whose data should be moved
    /// into the new ComponentData.
    ComponentData(ComponentData&& src) {
      entity = std::move(src.entity);
      data = std::move(src.data);
    }

    /// @brief Move a referenced ComponentData into this ComponentData.
    ///
    /// Move operators are efficient since they allow referenced data to
    /// be moved instead of copied.
    ///
    /// @param[in] src A referenced ComponentData to be moved into this
    /// ComponentData.
    ComponentData& operator=(ComponentData&& src) {
      entity = std::move(src.entity);
      data = std::move(src.data);
      return *this;
    }

   private:
    ComponentData(const ComponentData&);
    ComponentData& operator=(const ComponentData&);
  };

  /// @typedef EntityIterator
  ///
  /// @brief An iterator to iterate through all of the Entities in the
  /// System.
  typedef typename VectorPool<ComponentData>::Iterator EntityIterator;

  /// @typedef value_type
  ///
  /// @brief The templated data type stored by this System.
  typedef T value_type;

  /// @brief Construct a System without an EntityManager.
  System() : entity_manager_(nullptr) {}

  /// @brief Destructor for a System.
  virtual ~System() {}

  /// @brief Provides an alternate way to add Entities if you do not
  /// care about the returned data structure, and if you do not feel like
  /// casting the BaseComponent into something more specific.
  ///
  /// @note AddEntity is a much better function, except we cannot have it in
  /// the interface class, since it needs to know about type T for the return
  /// value.
  ///
  /// @param[in,out] entity An EntityRef reference used to add an Entity.
  virtual void AddEntityGenerically(EntityRef& entity) { AddEntity(entity); }

  /// @brief Adds an Entity to the list that this System is tracking.
  ///
  /// @param[in,out] entity An EntityRef reference used to add an Entity to the
  /// list of Entities that this System keeps track of.
  /// @param[in] alloc_location An Enum that specifies whether to allocate from
  /// the beginning or end of the memory pool.
  ///
  /// @return Returns the data structure associated with the System.
  ///
  /// @note If you have already registered for this System, this
  /// will just return a reference to the existing data and will not change
  /// anything.
  T* AddEntity(EntityRef& entity, AllocationLocation alloc_location) {
    if (HasDataForEntity(entity)) {
      return GetComponentData(entity);
    }
    // No existing data, so we allocate some and return it:
    SystemIndex index = static_cast<SystemIndex>(
        component_data_.GetNewElement(alloc_location).index());
    component_index_lookup_[entity->entity_id()] = index;
    ComponentData* component_data = component_data_.GetElementData(index);
    component_data->entity = entity;
    AddSystemDependencies(entity);
    InitEntity(entity);
    return &(component_data->data);
  }

  /// @brief Adds an Entity to the list that this System is tracking.
  ///
  /// @note Entities added through this function allocate from the back of the
  /// memory pool.
  ///
  /// @param[in,out] entity An EntityRef reference used to add an Entity to the
  /// list of Entities that this System keeps track of.
  ///
  /// @return Returns the data structure associated with the System.
  T* AddEntity(EntityRef& entity) { return AddEntity(entity, kAddToBack); }

  /// @brief Removes an Entity from the list of Entities.
  ///
  /// This is done by marking the Entity as no longer using this System,
  /// calling the destructor on the data, and returning the memory to the
  /// memory pool.
  ///
  /// @param[in,out] entity An EntityRef reference used to remove an Entity
  /// from the list of Entities that this System keeps track of.
  virtual void RemoveEntity(EntityRef& entity) {
    // Calling remove when there is no data is generally a sign that
    // something has gone wrong and that something has lost track of which
    // entities are associated with which components.  Use HasDataForEntity()
    // if you want to double-check if data exists before removing it.
    assert(HasDataForEntity(entity));
    RemoveEntityInternal(entity);
    component_data_.FreeElement(GetComponentDataIndex(entity));
    component_index_lookup_.erase(entity->entity_id());
  }

  /// @brief Removes an Entity from the list of Entities.
  ///
  /// This is done by marking the Entity as no longer using this System,
  /// calling the destructor on the data, and returning the memory to the
  /// memory pool.
  ///
  /// @param[in,out] iter An EntityIterator that references an Entity that
  /// should
  /// be removed.
  ///
  /// @return Returns an iterator to the next Entity after the one that was
  /// just removed.
  virtual EntityIterator RemoveEntity(EntityIterator iter) {
    EntityRef entity = iter->entity;
    RemoveEntityInternal(entity);
    EntityIterator new_iter = component_data_.FreeElement(iter);
    component_index_lookup_.erase(entity->entity_id());
    return new_iter;
  }

  /// @brief Gets an iterator that will iterate over every Entity associated
  /// with the System, starting from the beginning.
  ///
  /// @return Returns an iterator in the style of std that points to the first
  /// Entity in the list of all Entities in the System.
  virtual EntityIterator begin() { return component_data_.begin(); }

  /// @brief Gets an iterator that points to the end of the list of all entites
  /// in the System.
  ///
  /// @return Returns an iterator in the style of std that points to the last
  /// Entity in the list of all Entities in the System.
  virtual EntityIterator end() { return component_data_.end(); }

  /// @brief Updates all Entities. This is normally called, once per frame,
  /// by the EntityManager.
  virtual void UpdateAllEntities(WorldTime /*delta_time*/) {}

  /// @brief Checks if this component contains any data associated with the
  /// supplied entity.
  virtual bool HasDataForEntity(const EntityRef& entity) {
    return GetComponentDataIndex(entity) != kInvalidSystemIndex;
  }

  /// @brief Gets the data for a given Entity as a void pointer.
  ///
  /// @note When using GetSystemDataAsVoid, the calling function is expected
  /// to know how to handle the data (since it is returned as a void pointer).
  ///
  /// @warning This pointer is NOT stable in memory. Calls to AddEntity and
  /// AddEntityGenerically may force the storage class to resize,
  /// shuffling around the location of this data.
  ///
  /// @param[in] entity An EntityRef reference to the Entity whose data should
  /// be returned.
  ///
  /// @return Returns the Entity's data as a void pointer, or returns a nullptr
  /// if the data does not exist.
  virtual void* GetSystemDataAsVoid(const EntityRef& entity) {
    return GetComponentData(entity);
  }

  /// @brief Gets the data for a given Entity as a const void pointer.
  ///
  /// @note When using GetSystemDataAsVoid, the calling function is expected
  /// to know how to handle the data (since it is returned as a const
  /// void pointer).
  ///
  /// @warning This pointer is NOT stable in memory. Calls to AddEntity and
  /// AddEntityGenerically may force the storage class to resize,
  /// shuffling around the location of this data.
  ///
  /// @param[in] entity An EntityRef reference to the Entity whose data should
  /// be returned.
  ///
  /// @return Returns the Entity's data as a const void pointer, or returns a
  /// nullptr if the data does not exist.
  virtual const void* GetSystemDataAsVoid(const EntityRef& entity) const {
    return GetComponentData(entity);
  }

  /// @brief Gets the System data stored at a given index.
  ///
  /// @warning This pointer is NOT stable in memory. Calls to AddEntity and
  /// AddEntityGenerically may force the storage class to resize,
  /// shuffling around the location of this data.
  ///
  /// @param[in] data_index A size_t representing the index of the desired
  /// System data.
  ///
  /// @return Returns a pointer of the data structure associated with the
  /// System data, or returns a nullptr if given an invalid data_index.
  T* GetComponentData(size_t data_index) {
    if (data_index == kInvalidSystemIndex) {
      return nullptr;
    }
    ComponentData* element_data = component_data_.GetElementData(data_index);
    return (element_data != nullptr) ? &(element_data->data) : nullptr;
  }

  /// @brief Gets the data for a given Entity.
  ///
  /// @warning This pointer is NOT stable in memory. Calls to AddEntity and
  /// AddEntityGenerically may force the storage class to resize,
  /// shuffling around the location of this data.
  ///
  /// @param[in] entity An EntityRef reference to the Entity whose data should
  /// be returned.
  ///
  /// @return Returns the Entity's data as a pointer of the data structure
  /// associated with the System data, or returns a nullptr if the data
  /// does not exist.
  T* GetComponentData(const EntityRef& entity) {
    size_t data_index = GetComponentDataIndex(entity);
    if (data_index >= component_data_.Size()) {
      return nullptr;
    }
    return GetComponentData(data_index);
  }

  /// @brief Gets the System data stored at a given index.
  ///
  /// @warning This pointer is NOT stable in memory. Calls to AddEntity and
  /// AddEntityGenerically may force the storage class to resize,
  /// shuffling around the location of this data.
  ///
  /// @param[in] data_index A size_t representing the index of the desired
  /// System data.
  ///
  /// @return Returns a const pointer of the data structure associated with the
  /// System data, or returns a nullptr if given an invalid data_index.
  const T* GetComponentData(size_t data_index) const {
    return const_cast<System*>(this)->GetComponentData(data_index);
  }

  /// @brief Gets the data for a given Entity.
  ///
  /// @warning This pointer is NOT stable in memory. Calls to AddEntity and
  /// AddEntityGenerically may force the storage class to resize,
  /// shuffling around the location of this data.
  ///
  /// @param[in] entity An EntityRef reference to the Entity whose data should
  /// be returned.
  ///
  /// @return Returns the Entity's data as a const pointer of the data
  /// structure associated with the System data, or returns a nullptr
  /// if the data does not exist.
  const T* GetComponentData(const EntityRef& entity) const {
    return const_cast<System*>(this)->GetComponentData(entity);
  }

  /// @brief Clears all tracked System data.
  void virtual ClearComponentData() {
    for (auto iter = component_data_.begin(); iter != component_data_.end();
         iter = RemoveEntity(iter)) {
    }
  }

  /// @brief A utility function for retrieving the System data for an
  /// Entity from a specific System.
  ///
  /// @tparam ComponentDataType The data type of the System whose data
  /// is returned for the given Entity.
  ///
  /// @param[in] entity An EntityRef reference to the Entity whose System
  /// data should be returned.
  ///
  /// @return Returns a pointer to the data for the System of the given
  /// Entity or returns null if the Entity is not registered with the
  /// System.
  template <typename ComponentDataType>
  ComponentDataType* Data(const EntityRef& entity) {
    return entity_manager_->GetComponentData<ComponentDataType>(entity);
  }

  /// @brief A utility function for checking if an entity is registered with
  /// a particular component.
  ///
  /// @tparam ComponentDataType The data type of the System to be checked
  /// for registration.
  ///
  /// @param[in] entity An EntityRef reference to the Entity whose System
  /// data is checked.
  ///
  /// @return Returns true if the entity has been registered with the System,
  /// false otherwise.
  template <typename ComponentDataType>
  bool IsRegisteredWithComponent(const EntityRef& entity) {
    return entity_manager_
        ->GetSystem(entity_manager_->GetComponentId<ComponentDataType>())
        ->HasDataForEntity(entity);
  }

  /// @brief A utility function for retrieving the System data for an
  /// Entity from a specific System.
  ///
  /// @tparam ComponentDataType The data type of the System whose data
  /// is returned for the given Entity.
  ///
  /// @param[in] entity An EntityRef reference to the Entity whose System
  /// data should be returned.
  ///
  /// @return Returns a pointer to the data for the System of the given
  /// Entity or returns null if the Entity is not registered with the
  /// System.
  template <typename ComponentDataType>
  ComponentDataType* Data(const EntityRef& entity) const {
    return entity_manager_->GetComponentData<ComponentDataType>(entity);
  }

  /// @brief A utility function for retrieving a reference to a specific
  /// System object, by type.
  ///
  /// @tparam ComponentDataType The data type of the System.
  ///
  /// @return Returns a pointer to the data for a specific System.
  template <typename ComponentDataType>
  ComponentDataType* GetSystem() {
    return static_cast<ComponentDataType*>(entity_manager_->GetSystem(
        SystemIdLookup<ComponentDataType>::system_id));
  }

  // Virtual methods we inherited from component_interface:

  /// @brief Override this function with code that should be executed when
  /// the System is added to the EntityManager. (This typically
  /// happens once, at the beginning of the game before any Entities are
  /// added.)
  virtual void Init() {}

  /// @brief Invoked by EntityManager::FinalizeSystemList, once all systems
  // have been registered.  This is the place for systems to declare any
  /// dependencies they have on other systems, if any. (Via System::DependOn)
  virtual void DeclareDependencies() {}

  /// @brief A utility function for declaring a dependency on another system.
  ///
  /// @tparam ComponentDataType The data type of the System to depend on.
  template <typename SystemType>
  void DependOn(SystemDependencyType dependency_type) {
    DependOn(SystemIdLookup<SystemType>::system_id, dependency_type);
  }

  
  /// @brief A utility function for declaring a dependency on another system.
  ///
  /// @tparam ComponentDataType The data type of the System to depend on.
  virtual void DependOn(SystemId system_id, SystemDependencyType dependency_type) {
    if (dependency_type == kMustHaveComponent
      || dependency_type == kMustHaveComponentAndExecuteAfter
      || dependency_type == kMustHaveComponentAndExecuteBefore) {
      component_dependencies_.push_back(system_id);
    }

    if (dependency_type == kMustExecuteAfter
      || dependency_type == kMustHaveComponentAndExecuteAfter) {
      execute_dependencies_.push_back(system_id);
    }

    if (dependency_type == kMustExecuteBefore
      || dependency_type == kMustHaveComponentAndExecuteBefore) {

      assert(entity_manager_);
      assert(entity_manager_->is_system_list_final());
      SystemInterface* system = entity_manager_->GetSystem(system_id);
      assert(system);
      // If we need to make this system exceute before something else, then
      // we just add ourselves as a dependency to the target.  This way, the
      // dependency tree is entirely one-direction.
      system->DependOn(SystemIdLookup<T>::system_id, dependency_type == kMustExecuteBefore
                          ? kMustExecuteAfter 
                          : kMustHaveComponentAndExecuteAfter);
    }
  }

  /// @brief A utility function for registering an entity with all systems that
  /// this system depends on components from.  Will do nothing if it already has
  /// some or all of these components.
  ///
  /// @tparam entity The entity to add components to.
  void AddSystemDependencies(EntityRef& entity) {
    // Assert if we haven't finalized the system list yet...
    assert(entity_manager_);
    assert(entity_manager_->is_system_list_final());

    for (size_t i = 0; i < component_dependencies_.size(); i++) {
      SystemInterface* system = entity_manager_->GetSystem(component_dependencies_[i]);
      assert(system);
      system->AddEntityGenerically(entity);
    }
  }

  /// @brief Override this function with code that should be executed when an
  /// Entity is added to the System.
  virtual void InitEntity(EntityRef& /*entity*/) {}

  /// @brief Creates and populates an Entity from raw data. Components that want
  /// to be able to be constructed via the EntityFactory need to implement this.
  ///
  /// @param[in,out] entity An EntityRef that points to an Entity that is being
  /// added from the raw data.
  /// @param[in] data A void pointer to the raw data.
  virtual void AddFromRawData(EntityRef& entity, const void* data) {}

  /// @brief Override this function to return raw data that can be read back
  /// later.
  ///
  /// @warning By default, Components do not support this functionality. If you
  /// wish to support this, you will need to override this function.
  ///
  /// @return By default, this returns a nullptr.
  virtual RawDataUniquePtr ExportRawData(const EntityRef& /*unused*/) const {
    return nullptr;
  }

  /// @brief Override this function with any code that executes when this
  /// System is removed from the EntityManager. (i.e. Usually when the
  /// game/state is over and everything is shutting down.)
  virtual void Cleanup() {}

  /// @brief Override this function with any code that needs to be executed
  /// when an Entity is removed from this System.
  virtual void CleanupEntity(EntityRef& /*entity*/) {}

  /// @brief Set the EntityManager for this System.
  ///
  /// @note The EntityManager is used as the main point of contact
  /// for Components that need to talk to other things.
  ///
  /// @param[in] entity_manager A pointer to an EntityManager to associate
  /// with this System.
  virtual void SetEntityManager(EntityManager* entity_manager) {
    entity_manager_ = entity_manager;
  }

  /// @brief Get the ID for this System.
  ///
  /// @return Returns the System ID for this System.
  static SystemId GetComponentId() {
    return static_cast<SystemId>(SystemIdLookup<T>::system_id);
  }

  /// @brief Sets the System ID on the data type.
  ///
  /// @note This is usually only called by the EntityManager.
  ///
  /// @param[in] id The System ID to set on the data type.
  virtual void SetSystemIdOnDataType(SystemId id) {
    SystemIdLookup<T>::system_id = id;
  }

 private:
  /// @brief Allows Components to handle any per-Entity clean up that may
  /// be needed.
  ///
  /// @param[in] entity An EntityRef reference to the Entity that is being
  /// removed and may need to be cleaned up.
  void RemoveEntityInternal(EntityRef& entity) { CleanupEntity(entity); }

  /// @brief : List of systems that we depend on the components of:
  std::vector<corgi::SystemId> component_dependencies_;

  /// @brief : List of systems that we depend on having updated first,
  /// before this system updates.
  std::vector<corgi::SystemId> execute_dependencies_;

 protected:
  /// @brief Get the index of the System data for a given Entity.
  ///
  /// @param[in] entity An EntityRef reference to the Entity whose data
  /// index will be returned.
  ///
  /// @return Returns a size_t corresponding to the index of the
  /// System data, or kInvalidSystemIndex if no data could be found.
  size_t GetComponentDataIndex(const EntityRef& entity) const {
    auto result = component_index_lookup_.find(entity->entity_id());
    return (result != component_index_lookup_.end()) ? result->second
                                                     : kInvalidSystemIndex;
  }

  /// @var component_data_
  ///
  /// @brief Storage for all of the data for the System.
  VectorPool<ComponentData> component_data_;

  /// @var entity_manager_
  ///
  /// @brief A pointer to the EntityManager for this System. This is the
  /// main point of contact for Components that need to talk to other things.
  EntityManager* entity_manager_;

  /// @var component_index_lookup_
  ///
  /// @brief A map for translating unique entity IDs into vectorpool
  /// indexes.
  std::unordered_map<EntityIdType, SystemIndex> component_index_lookup_;
};
/// @}

}  // corgi

#endif  // CORGI_COMPONENT_H_