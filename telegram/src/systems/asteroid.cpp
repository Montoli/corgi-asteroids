#include <SDL.h>
#include "asteroid.h"
#include "transform.h"
#include "sprite.h"
#include "physics.h"
#include "wallbounce.h"
#include <stdio.h>
#include "constants.h"
#include "bullet.h"
#include <unordered_set>


CORGI_DEFINE_SYSTEM(AsteroidSystem, AsteroidData)

void AsteroidSystem::UpdateAllEntities(corgi::WorldTime delta_time) {

  if (begin() == end()) {
    for (int i = 0; i < 2; i++) {
      corgi::Entity new_asteroid = entity_manager_->AllocateNewEntity();
      entity_manager_->AddComponent<AsteroidSystem>(new_asteroid);
    }
  }

  BulletSystem* bullet_system = GetSystem<BulletSystem>();
  for (auto itr = begin(); itr != end(); ++itr) {
    bullet_system->CheckForAsteroidHit(itr->entity);
  }
}

void AsteroidSystem::DeclareDependencies() {
	DependOn<SpriteSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess, corgi::kAutoAdd);
	DependOn<TransformSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess, corgi::kAutoAdd);
  DependOn<PhysicsSystem>(corgi::kExecuteAfter, corgi::kReadWriteAccess, corgi::kAutoAdd);
  DependOn<WallBounceSystem>(corgi::kNoOrderDependency, corgi::kReadAccess, corgi::kAutoAdd);

  DependOn<BulletSystem>(corgi::kExecuteAfter, corgi::kNoAccessDependency, corgi::kNoAutoAdd);

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

void AsteroidSystem::ApplyDamage(corgi::Entity asteroid, float damage) {
  AsteroidData* data = Data<AsteroidData>(asteroid);
  float radius = data->radius;
  TransformData* transform = Data<TransformData>(asteroid);

  data->hp -= damage;
  if (data->hp <= 0) {
    data->hp = 100;
    if (radius > 15.0f) {
      // Spawn some new asteroids!
      for (int i = 0; i < 3; i++) {
        corgi::Entity new_asteroid = entity_manager_->AllocateNewEntity();
        entity_manager_->AddComponent<AsteroidSystem>(new_asteroid);

        // Pointers to data are not guaranteed to be valid,
        // an entity allocation, or component assignment, so we
        // need to fetch these again:
        transform = Data<TransformData>(asteroid);

        float new_radius = (rnd() * 0.25f + 0.4f) * radius;
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