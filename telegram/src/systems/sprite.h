#ifndef SPRITE_SYSTEM_H
#define SPRITE_SYSTEM_H
#include <SDL.h>
#include <SDL_image.h>
#include "corgi/system.h"
#include "math_common.h"
#include "GL/glew.h"



struct SpriteComponent {
	vec3 position;
	vec3 scale;
	int texture;
	vec4 tint;
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

	GLuint shader_program;

};

CORGI_REGISTER_SYSTEM(SpriteSystem, SpriteComponent)


#endif // SPRITE_SYSTEM_H