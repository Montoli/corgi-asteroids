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

void BulletSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
  for (int i = 0; i < kBucketRows * kBucketColumns; i++) {
    collision_map[i].clear();
  }
  for (auto itr = begin(); itr != end(); ++itr) {
    TransformData* transform = Data<TransformData>(itr->entity);
    if (transform->position.x() < 0 ||
        transform->position.y() < 0 ||
        transform->position.x() >= kScreenWidth ||
        transform->position.y() >= kScreenHeight) {
      entity_manager_->DeleteEntity(itr->entity);
    } else {
      // Store a reference to each bullet in a "bucket"
      // representing the general area it lives on the
      // screen.  (For speeding up collision detections later.)

      int bucket_index = GetBucketIndex(transform->position.xy());
      collision_map[bucket_index].insert(itr->entity);
    }
  }
}


void BulletSystem::CheckForAsteroidHit(corgi::Entity asteroid_entity) {
  AsteroidData* asteroid = Data<AsteroidData>(asteroid_entity);
  TransformData* transform = Data<TransformData>(asteroid_entity);
  
  int top    = (transform->position.y() - asteroid->radius) / kBucketSize;
  int left   = (transform->position.x() - asteroid->radius) / kBucketSize;
  int bottom = (transform->position.y() + asteroid->radius) / kBucketSize;
  int right  = (transform->position.x() + asteroid->radius) / kBucketSize;

  if (top < 0) top = 0;
  if (bottom >= kBucketRows) bottom = kBucketRows - 1;
  if (left < 0) left = 0;
  if (right >= kBucketColumns) right = kBucketColumns - 1;

  for (int x = left; x <= right; x++) {
    for (int y = top; y <= bottom; y++) {
      int bucket_index = x + y * kBucketColumns;
      for (auto itr = collision_map[bucket_index].begin();
          itr != collision_map[bucket_index].end(); ++itr) {

        if (!entity_manager_->IsEntityMarkedForDeletion(*itr)) {

          TransformData* bullet_transform = Data<TransformData>(*itr);

          vec2 diff = transform->position.xy() - bullet_transform->position.xy();
          float dist_squared = diff.x() * diff.x() + diff.y() * diff.y();

          float rad_squared = (kBulletRadius + asteroid->radius) *
            (kBulletRadius + asteroid->radius);

          if (dist_squared < rad_squared) {
            SpawnHitSparks(*itr);

            GetSystem<AsteroidSystem>()->ApplyDamage(
                asteroid_entity, kBulletDamage);

            entity_manager_->DeleteEntity(*itr);
          }
        }
      }
    }
  }
}



void BulletSystem::DeclareDependencies() {
	DependOn<SpriteSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
	DependOn<TransformSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
  DependOn<PhysicsSystem>(corgi::kExecuteAfter, corgi::kReadWriteAccess);
  DependOn<AsteroidSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
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

int BulletSystem::GetBucketIndex(vec2 position) {
  int column = position.x() / kBucketSize;
  int row = position.y() / kBucketSize;

  if (row < 0) row = 0;
  if (row >= kBucketRows) row = kBucketRows - 1;
  if (column < 0) column = 0;
  if (column >= kBucketColumns) column = kBucketColumns - 1;

  int index = column + row * kBucketColumns;
  assert(index >= 0);
  assert(index < kBucketColumns * kBucketColumns);

  return index;
}



void BulletSystem::CleanupEntity(corgi::Entity entity) {
  /*
  TransformData* transform = Data<TransformData>(entity);
  int bucket_index = GetBucketIndex(transform->position.xy());
  collision_map[bucket_index].erase(entity);
  */
}
