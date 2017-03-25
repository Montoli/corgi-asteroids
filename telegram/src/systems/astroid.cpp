#include <SDL.h>
#include "asteroid.h"
#include "transform.h"
#include "sprite.h"
#include "physics.h"
#include "wallbounce.h"
#include <stdio.h>


CORGI_DEFINE_SYSTEM(AsteroidSystem, AsteroidData)

void AsteroidSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
	//printf("entity updated!");
  // TODO: If there are 0 asteroids, spawn 5 or whatever.

  /*
  if (component_data_.size() <= 0) {
    for (int i = 0; i < 15; i++) {
      corgi::Entity new_asteroid = entity_manager_->AllocateNewEntity();
      entity_manager_->AddComponent<AsteroidSystem>(new_asteroid);
    }
  }
  */


  for (auto itr = begin(); itr != end(); ++itr) {
    /*
    corgi::Entity entity = itr->entity;

		TransformData* transform = Data<TransformData>(entity);
		SpriteData* sprite = Data<SpriteData>(entity);
		PhysicsData* physics = Data<PhysicsData>(entity);

		transform->scale *= 0.98f;
		if (transform->scale.x() < 0.1) {
			entity_manager_->DeleteEntity(entity);
		}*/
    //TestData* testdata = Data<TestData>(entity);
  }

}

void AsteroidSystem::DeclareDependencies() {
	DependOn<SpriteSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
	DependOn<TransformSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
  DependOn<PhysicsSystem>(corgi::kExecuteAfter, corgi::kReadWriteAccess);
  DependOn<WallBounceSystem>(corgi::kNoOrderDependency, corgi::kReadAccess);
  SetIsThreadSafe(true);
}


void AsteroidSystem::InitEntity(corgi::Entity entity) {

  const char* asteroid_texture = "rsc/asteroid.png";
  const char* asteroid_texture2 = "rsc/ship.png";

  AsteroidData* asteroid = Data<AsteroidData>(entity);
  asteroid->radius = rnd() * 20.0f + 40.0f;

  TransformData* transform = Data<TransformData>(entity);
  transform->origin = vec2(0.5f, 0.5f);
  transform->position = vec3(rnd() * 640, rnd() * 480, 0);
  transform->scale = vec2(asteroid->radius, asteroid->radius);

  SpriteData* sprite = Data<SpriteData>(entity);
  PhysicsData* physics = Data<PhysicsData>(entity);

  sprite->size = vec2(1, 1);
  sprite->tint = vec4(0.5f + rnd(), 0.5f + rnd(), 0.5f + rnd(), 1.0f);
  sprite->texture = asteroid_texture;

  if (rnd() > 0.5f) {
    sprite->texture = asteroid_texture2;

  }


  physics->angular_velocity = quat::FromAngleAxis(rnd() * 0.1f - 0.05f, vec3(0.0f, 0.0f, 1.0f));

}