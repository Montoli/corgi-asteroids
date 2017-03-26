#include <SDL.h>
#include "playership.h"
#include "transform.h"
#include "sprite.h"
#include "physics.h"
#include "wallbounce.h"
#include <stdio.h>
#include "common.h"
#include "fade_timer.h"


CORGI_DEFINE_SYSTEM(PlayerShip, PlayerShipData)


static const float kTurnSpeed = -0.1f;
static const float kThrust = 0.1f;
static const float kMaxSpeed = 5.0f;

static const float kExhaustSpeed = -3.0f;

// basic orientation is pointing straight up.
static const vec3 kBaseOrientation = vec3(0, 1, 0);

void PlayerShip::UpdateAllEntities(corgi::WorldTime delta_time) {

  CommonComponent* common = GetSystem<CommonSystem>()->CommonData();

  for (auto itr = begin(); itr != end(); ++itr) {
    // In theory this iterator is silly, since there will usually
    // only be only one player.  But who knows what we'll want
    // in the future!
    TransformData* transform = Data<TransformData>(itr->entity);
    PhysicsData* physics = Data<PhysicsData>(itr->entity);
    vec3& pos = transform->position;
    quat& rotation = transform->orientation;
    vec2& velocity = physics->velocity;
    vec3 heading = rotation * kBaseOrientation;
    if (common->keyboard_input->GetKeyState(SDLK_LEFT).is_down) {
      rotation = rotation * quat::FromAngleAxis(-kTurnSpeed, vec3(0, 0, 1));
    }
    if (common->keyboard_input->GetKeyState(SDLK_RIGHT).is_down) {
      rotation = rotation * quat::FromAngleAxis(kTurnSpeed, vec3(0, 0, 1));
    }
    if (common->keyboard_input->GetKeyState(SDLK_UP).is_down) {
      velocity += vec2(heading.x(), -heading.y()) * kThrust;
      SpawnExhaust(itr->entity);
      //rotation = rotation * quat::FromAngleAxis(kTurnSpeed, vec3(0, 0, 1));
    }



  }
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

void PlayerShip::SpawnExhaust(corgi::Entity ship) {
  const char* texture_path = "rsc/circle.png";
  corgi::Entity exhaust = entity_manager_->AllocateNewEntity();
  entity_manager_->AddComponent<SpriteSystem>(exhaust);
  entity_manager_->AddComponent<FadeTimerSystem>(exhaust);
  entity_manager_->AddComponent<PhysicsSystem>(exhaust);

  SpriteData* sprite_data = Data<SpriteData>(exhaust);
  FadeTimerData* fade_data = Data<FadeTimerData>(exhaust);
  PhysicsData* physics_data = Data<PhysicsData>(exhaust);
  TransformData* transform_data = Data<TransformData>(exhaust);

  TransformData* ship_transform = Data<TransformData>(ship);
  vec2 heading = (ship_transform->orientation * kBaseOrientation).xy().Normalized();
  heading.y() = -heading.y();

  sprite_data->size = vec2(10, 10);
  transform_data->origin = vec2(5, 5);
  transform_data->position = ship_transform->position + vec3(heading * -15.0f, 0);
  sprite_data->tint = vec4(1, 1, 0, 1);
  sprite_data->texture = texture_path;


  fade_data->counter = 500.0f;
  fade_data->fade_point = 500.0f;
  physics_data->velocity = kExhaustSpeed * heading;

}

void PlayerShip::DeclareDependencies() {
	DependOn<SpriteSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
  DependOn<PhysicsSystem>(corgi::kExecuteAfter, corgi::kReadWriteAccess);
  DependOn<WallBounceSystem>(corgi::kNoOrderDependency, corgi::kReadAccess);
  DependOn<TransformSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);

  RequireComponent<SpriteSystem>();
  RequireComponent<PhysicsSystem>();
  RequireComponent<WallBounceSystem>();
  RequireComponent<TransformSystem>();
  DependOn<FadeTimerData>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
  SetIsThreadSafe(true);
}


void PlayerShip::InitEntity(corgi::Entity entity) {
  const char* texture_path = "rsc/ship.png";

  PlayerShipData* ship_data = Data<PlayerShipData>(entity);

  TransformData* transform = Data<TransformData>(entity);
  transform->origin = vec2(15.0f, 15.0f);
  transform->position = vec3(rnd() * 640, rnd() * 480, 0);
  
  SpriteData* sprite = Data<SpriteData>(entity);
  PhysicsData* physics = Data<PhysicsData>(entity);

  sprite->size = vec2(30, 30);
  sprite->texture = texture_path;
}