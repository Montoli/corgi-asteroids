#include <SDL.h>
#include "asteroid.h"
#include "bullet.h"
#include "transform.h"
#include "sprite.h"
#include "physics.h"
#include "wallbounce.h"
#include <stdio.h>
#include "constants.h"
#include "fade_timer.h"


CORGI_DEFINE_SYSTEM(BulletSystem, BulletData)

const float kBulletRadius = 2.5f;
const float kBulletDamage = 2.0f;

void BulletSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
  for (auto itr = begin(); itr != end(); ++itr) {
    TransformData* transform = Data<TransformData>(itr->entity);
    if (transform->position.x() < 0 ||
        transform->position.y() < 0 ||
        transform->position.x() >= kScreenWidth ||
        transform->position.y() >= kScreenHeight) {
      entity_manager_->DeleteEntity(itr->entity);
    }

    AsteroidSystem* asteroid_system = GetSystem<AsteroidSystem>();
    corgi::Entity hit = asteroid_system->CollisionCheck(
        transform->position.xy(), kBulletRadius);
    if (hit != corgi::kInvalidEntityId) {
      asteroid_system->ApplyDamage(hit, kBulletDamage);
      SpawnHitSparks(itr->entity);
      entity_manager_->DeleteEntity(itr->entity);
    }

  }
}

void BulletSystem::DeclareDependencies() {
	DependOn<SpriteSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
	DependOn<TransformSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
  DependOn<PhysicsSystem>(corgi::kExecuteAfter, corgi::kReadWriteAccess);
  DependOn<AsteroidSystem>(corgi::kExecuteAfter, corgi::kReadWriteAccess);
  DependOn<FadeTimerSystem>(corgi::kExecuteAfter, corgi::kReadWriteAccess);

  RequireComponent<SpriteSystem>();
  RequireComponent<TransformSystem>();
  RequireComponent<PhysicsSystem>();
  SetIsThreadSafe(true);
}


void BulletSystem::SpawnHitSparks(corgi::Entity bullet) {
  const char* texture_path = "rsc/circle.png";
  corgi::Entity spark = entity_manager_->AllocateNewEntity();
  entity_manager_->AddComponent<SpriteSystem>(spark);
  entity_manager_->AddComponent<FadeTimerSystem>(spark);
  entity_manager_->AddComponent<PhysicsSystem>(spark);

  SpriteData* sprite_data = Data<SpriteData>(spark);
  FadeTimerData* fade_data = Data<FadeTimerData>(spark);
  PhysicsData* physics_data = Data<PhysicsData>(spark);
  TransformData* transform_data = Data<TransformData>(spark);

  TransformData* bullet_transform = Data<TransformData>(bullet);

  sprite_data->size = vec2(20, 20);
  transform_data->origin = vec2(10, 10);
  transform_data->position = bullet_transform->position;
  transform_data->position.z() = kLayerParticles;

  sprite_data->tint = vec4(1.0f, 0.5f + rnd() * 0.5f, 0, 1.0f);
  sprite_data->texture = texture_path;

  fade_data->counter = 100.0f;
  fade_data->fade_point = 100.0f;
  physics_data->velocity = vec2(rnd() * 5.0f - 2.5f, rnd() * 5.0f - 2.5f);

}


void BulletSystem::InitEntity(corgi::Entity entity) {

  const char* bullet_texture = "rsc/circle.png";

  TransformData* transform = Data<TransformData>(entity);
  transform->origin = vec2(0.5f, 0.5f);
  transform->position = vec3(rnd() * 640, rnd() * 480, kLayerAsteroids);
  transform->scale = vec2(5, 5);

  SpriteData* sprite = Data<SpriteData>(entity);
  PhysicsData* physics = Data<PhysicsData>(entity);

  sprite->size = vec2(1, 1);
  sprite->tint = vec4(0.5f + rnd(), 0.5f + rnd(), 0.5f + rnd(), 1.0f);
  sprite->texture = bullet_texture;

  physics->angular_velocity = quat::FromAngleAxis(rnd() * 0.1f - 0.05f, vec3(0.0f, 0.0f, 1.0f));
  physics->velocity = vec2(rnd() * 1.0f - 0.5f, rnd() * 1.0f - 0.5f);
}