#include <SDL.h>
#include <stdio.h>
#include "common.h"
#include "transform.h"
#include "physics.h"
#include "wallbounce.h"


CORGI_DEFINE_SYSTEM(WallBounceSystem, WallBounceData)

void WallBounceSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
  CommonComponent* common = entity_manager_->GetSystem<CommonSystem>()->CommonData();

  for (auto itr = begin(); itr != end(); ++itr) {
    corgi::Entity entity = itr->entity; 
    TransformData* transform = Data<TransformData>(entity);
    PhysicsData* physics = Data<PhysicsData>(entity);

    if (transform->position.x() < 0) {
      transform->position.x() = 0;
      if (physics->velocity.x() < 0) physics->velocity.x() *= -1;
    }
    if (transform->position.y() < 0) {
      transform->position.y() = 0;
      if (physics->velocity.y() < 0) physics->velocity.y() *= -1;
    }
    if (transform->position.x() >= common->screen_size.x()) {
      transform->position.x() = common->screen_size.x() - 1.0f;
      if (physics->velocity.x() > 0) physics->velocity.x() *= -1;
    }
    if (transform->position.y() >= common->screen_size.y()) {
      transform->position.y() = common->screen_size.y() - 1.0f;
      if (physics->velocity.y() > 0) physics->velocity.y() *= -1;
    }
  }
}

void WallBounceSystem::DeclareDependencies() {
	DependOn<TransformSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
	DependOn<PhysicsData>(corgi::kExecuteAfter, corgi::kReadWriteAccess);
	SetIsThreadSafe(true);
}

