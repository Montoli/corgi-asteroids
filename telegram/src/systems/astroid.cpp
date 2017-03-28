#include <SDL.h>
#include "asteroid.h"
#include "transform.h"
#include "sprite.h"
#include "physics.h"
#include "wallbounce.h"
#include <stdio.h>
#include "constants.h"


CORGI_DEFINE_SYSTEM(AsteroidSystem, AsteroidData)

void AsteroidSystem::UpdateAllEntities(corgi::WorldTime delta_time) {

  if (begin() == end()) {
    for (int i = 0; i < 2; i++) {
      corgi::Entity new_asteroid = entity_manager_->AllocateNewEntity();
      entity_manager_->AddComponent<AsteroidSystem>(new_asteroid);
    }
  }

  //for (auto itr = begin(); itr != end(); ++itr) {
    // anything?
  //}
}

void AsteroidSystem::DeclareDependencies() {
	DependOn<SpriteSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
	DependOn<TransformSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
  DependOn<PhysicsSystem>(corgi::kExecuteAfter, corgi::kReadWriteAccess);
  DependOn<WallBounceSystem>(corgi::kNoOrderDependency, corgi::kReadAccess);

  RequireComponent<SpriteSystem>();
  RequireComponent<TransformSystem>();
  RequireComponent<PhysicsSystem>();
  RequireComponent<WallBounceSystem>();
  SetIsThreadSafe(true);
}


void AsteroidSystem::InitEntity(corgi::Entity entity) {

  const char* asteroid_texture = "rsc/asteroid.png";

  AsteroidData* asteroid = Data<AsteroidData>(entity);

  TransformData* transform = Data<TransformData>(entity);
  transform->origin = vec2(0.5f, 0.5f);
  transform->position = vec3(rnd() * 640, rnd() * 480, kLayerAsteroids);
  transform->scale = vec2(asteroid->radius * 2.0f, asteroid->radius * 2.0f);

  SpriteData* sprite = Data<SpriteData>(entity);
  PhysicsData* physics = Data<PhysicsData>(entity);

  sprite->size = vec2(1, 1);
  sprite->tint = vec4(0.5f + rnd(), 0.5f + rnd(), 0.5f + rnd(), 1.0f);
  sprite->texture = asteroid_texture;

  physics->angular_velocity = quat::FromAngleAxis(rnd() * 0.1f - 0.05f, vec3(0.0f, 0.0f, 1.0f));
  physics->velocity = vec2(rnd() * 1.0f - 0.5f, rnd() * 1.0f - 0.5f);
}


corgi::Entity AsteroidSystem::CollisionCheck(vec2 position, float radius) {
  for (auto itr = begin(); itr != end(); ++itr) {
    TransformData* transform = Data<TransformData>(itr->entity);
    AsteroidData* asteroid = Data<AsteroidData>(itr->entity);

    vec2 diff = transform->position.xy() - position;
    float dist_squared = diff.x() * diff.x() + diff.y() * diff.y();

    float rad_squared = (radius + asteroid->radius) * (radius + asteroid->radius);

    if (dist_squared < rad_squared) {
      return itr->entity;
    }
  }
  return corgi::kInvalidEntityId;

}


void AsteroidSystem::ApplyDamage(corgi::Entity asteroid, float damage) {
  AsteroidData* data = Data<AsteroidData>(asteroid);
  TransformData* transform = Data<TransformData>(asteroid);

  data->hp -= damage;
  if (data->hp < 0) {
    if (data->radius > 15.0f) {
      // Spawn some new asteroids!
      for (int i = 0; i < 3; i++) {
        corgi::Entity new_asteroid = entity_manager_->AllocateNewEntity();
        entity_manager_->AddComponent<AsteroidSystem>(new_asteroid);

        // Pointers to data are not guaranteed to be valid,
        // an entity allocation, or component assignment, so we
        // need to fetch these again:
        data = Data<AsteroidData>(asteroid);
        transform = Data<TransformData>(asteroid);


        float new_radius = (rnd() * 0.25f + 0.4f) * data->radius;

        AsteroidData* new_asteroid_data = Data<AsteroidData>(new_asteroid);
        new_asteroid_data->radius = new_radius;
        new_asteroid_data->hp = new_radius * kHpScale;

        TransformData* new_transform = Data<TransformData>(new_asteroid);
        new_transform->scale = vec2(new_asteroid_data->radius * 2.0f, new_asteroid_data->radius * 2.0f);
        new_transform->position = transform->position;

        Data<PhysicsData>(new_asteroid)->velocity = vec2(
          rnd() * (100.0f / new_radius), rnd() * (100.0f / new_radius));
      }
    }
    entity_manager_->DeleteEntity(asteroid);
  }


}