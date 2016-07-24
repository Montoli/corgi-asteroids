#ifndef SPRITE_SYSTEM_H
#define SPRITE_SYSTEM_H
#include <SDL.h>
#include <SDL_image.h>
#include "corgi/system.h"
#include "math_common.h"
#include "GL/glew.h"

static const int kMaxSprites = 500;

// 4 points, each point contains 3 axis coordinates, 2 UV coordinates, and 4 tint values.
//static const int kSpriteSize = (sizeof(float) * (3 + 2 + 4) * 4);
static const int kFloatsPerPoint = 3 + 2 + 4;



static const int kVertexLoc = 0;
static const int kTextureUVLoc = 1;
static const int kTintLoc = 2;

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



	GLfloat* vertex_buffer[kMaxSprites * kFloatsPerPoint];

};

CORGI_REGISTER_SYSTEM(SpriteSystem, SpriteComponent)


#endif // SPRITE_SYSTEM_H