#ifndef SPRITE_SYSTEM_H
#define SPRITE_SYSTEM_H
#include <SDL.h>
#include <SDL_image.h>
#include "corgi/system.h"

struct SpriteComponent {
  int num = 42;
};


class SpriteSystem : public corgi::System<SpriteComponent> {
public:

	virtual void Init();

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();

	virtual void Cleanup();

	virtual void InitEntity(corgi::EntityRef& entity);

	void RenderSprites();

	SDL_Surface * LoadPNG(std::string path);

private:
	SDL_Surface* hello_world = NULL;

};

CORGI_REGISTER_SYSTEM(SpriteSystem, SpriteComponent)


#endif // SPRITE_SYSTEM_H