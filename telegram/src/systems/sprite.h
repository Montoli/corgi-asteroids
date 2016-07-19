#ifndef SPRITE_SYSTEM_H
#define SPRITE_SYSTEM_H
#include "corgi/system.h"

struct SpriteComponent {
  int num = 42;
};


class SpriteSystem : public corgi::System<SpriteComponent> {
public:

	virtual void Init();

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();

  virtual void InitEntity(corgi::EntityRef& entity);

};

CORGI_REGISTER_SYSTEM(SpriteSystem, SpriteComponent)


#endif // SPRITE_SYSTEM_H