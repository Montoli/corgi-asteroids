#include <SDL.h>
#include "fountain_projectile.h"
#include "transform.h"
#include "sprite.h"
#include "physics.h"
#include <stdio.h>


CORGI_DEFINE_SYSTEM(FountainProjectile, FountainData)

void FountainProjectile::UpdateAllEntities(corgi::WorldTime delta_time) {
	//printf("entity updated!");
  for (auto itr = begin(); itr != end(); ++itr) {
    corgi::EntityRef entity = itr->entity;

		TransformData* transform = Data<TransformData>(entity);
		SpriteData* sprite = Data<SpriteData>(entity);
		PhysicsData* physics = Data<PhysicsData>(entity);

		transform->scale *= 0.98f;
		if (transform->scale.x() < 0.1) {
			entity_manager_->DeleteEntity(entity);
		}
    //TestData* testdata = Data<TestData>(entity);
  }

}

void FountainProjectile::DeclareDependencies() {
	DependOn<SpriteSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
	DependOn<TransformSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
	DependOn<PhysicsData>(corgi::kExecuteAfter, corgi::kReadWriteAccess);
	SetIsThreadSafe(true);
}


void FountainProjectile::InitEntity(corgi::EntityRef& entity) {
	TransformData* transform = Data<TransformData>(entity);
	SpriteData* sprite = Data<SpriteData>(entity);
	PhysicsData* physics = Data<PhysicsData>(entity);

	
	transform->position = vec3(320, 480, 0);
	transform->scale = vec2(rnd() + 0.5f, rnd() + 0.5f);

	transform->origin = vec2(25, 25);

	sprite->size = vec2(50, 50);
	sprite->tint = vec4(0.5f + rnd(), 0.5f + rnd(), 0.5f + rnd(), 1.0f);

	physics->velocity = vec2(rnd() * 6.0f - 3.0f, rnd() * -5.0f);
	physics->acceleration = vec2(0.0f, 0.1f);


}