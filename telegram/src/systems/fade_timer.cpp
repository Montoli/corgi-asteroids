#include "fade_timer.h"
#include "sprite.h"

CORGI_DEFINE_SYSTEM(FadeTimerSystem, FadeTimerData)

void FadeTimerSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
  for (auto itr = begin(); itr != end(); ++itr) {
    corgi::Entity entity = itr->entity;
    SpriteData* sprite = Data<SpriteData>(entity);
    FadeTimerData* fade_data = Data<FadeTimerData>(entity);

    fade_data->counter -= delta_time;
    if (fade_data->counter < fade_data->fade_point) {
      sprite->tint.w() = fade_data->counter / fade_data->fade_point;
    }

    if (fade_data->counter <= 0) {
      entity_manager_->DeleteEntity(entity);
    }
  }
}

void FadeTimerSystem::DeclareDependencies() {
  DependOn<SpriteData>(corgi::kExecuteBefore,
      corgi::kReadWriteAccess, corgi::kAutoAdd);
  SetIsThreadSafe(true);
}

