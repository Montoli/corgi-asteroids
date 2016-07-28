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

#include <stddef.h>
#include <stdint.h>

#ifndef CORGI_ENTITY_COMMON_H_
#define CORGI_ENTITY_COMMON_H_

#define CORGI_ENFORCE_SYSTEM_DEPENDENCIES

namespace corgi {

/// @file
/// @addtogroup corgi_entity
/// @{
///

	/// @var SystemId
///
/// @brief This represents the ID of a System.
///
/// @note Depending on kMaxComponentCount, this value is either an uint8_t,
/// uint16_t, or uint32_t. (However, if you need an uint32_t, you are
/// probably doing something odd.)
/// @{
typedef uint16_t SystemId;
/// @}

/// @var kInvalidSystem
///
/// @brief A sentinel value to represent an invalid System.
///
/// @note System IDs start at 1.
const SystemId kInvalidSystem = static_cast<SystemId>(-1);

/// @typedef WorldTime
///
/// @brief A typedef that represents time in the game.
typedef double WorldTime;

/// @typedef ComponentIndex
///
/// @brief A ComponentIndex is a value used to represent the location of a piece
/// of ComponentData, normally inside of a VectorPool.
typedef uint16_t ComponentIndex;

/// @typedef EntityIdType
///
/// @brief A EntityIdType is a value used to uniquely represent an entity
/// in various internal structures.  In general, CORGI users should avoid
/// using this directly, and should instead refer to entities via EntityRefs.
typedef uint32_t EntityIdType;

typedef EntityIdType Entity;

/// @var kInvalidEntityId
///
/// @brief A sentinel value to represent an invalid entity.
///
/// Entities have a unique ID that is used reference them internally.  This
/// value represents an invalid entity.  It is usually either used for
/// uninitialized values, or to indicate a null return value.
static const EntityIdType kInvalidEntityId = static_cast<EntityIdType>(-1);

/// @enum SystemDependency
///
/// @brief `Various ways that systems depend on each other.
enum SystemOrderDependencyType {
  kNoOrderDependency = 0,
  kExecuteBefore,
  kExecuteAfter,
};

enum SystemAccessDependencyType {
  kNoAccessDependency = 0,
  kReadAccess,
  kReadWriteAccess,
};

/// @}

}  // corgi

#endif  // CORGI_ENTITY_COMMON_H_
