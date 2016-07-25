#ifndef SPRITE_SYSTEM_H
#define SPRITE_SYSTEM_H
#include <SDL.h>
#include <SDL_image.h>
#include "corgi/system.h"
#include "math_common.h"
#include "GL/glew.h"

struct SpriteData {
	//int texture; // make this something real!
	vec2 uv;
	vec2 size;
	vec4 tint;
};


class SpriteSystem : public corgi::System<SpriteData> {
public:

	virtual void Init();

	virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();

	virtual void Cleanup();

	virtual void InitEntity(corgi::EntityRef& entity);

	void RenderSprites();

	SDL_Surface * LoadPNG(std::string path);

private:
	void AddPointToBuffer(vec4 p, vec2 uv, vec4 tint);

	static const int kMaxSprites = 500;
	static const int kPointsPerSprite = 6;

	// 4 points, each point contains 3 axis coordinates, 2 UV coordinates, and 4 tint values.
	//static const int kSpriteSize = (sizeof(float) * (3 + 2 + 4) * 4);
	static const int kFloatsPerPoint = 3 + 2 + 4;

	static const int kTotalBufferSize = kMaxSprites * kPointsPerSprite * kFloatsPerPoint;

	static const int kVertexLoc = 0;
	static const int kTextureUVLoc = 1;
	static const int kTintLoc = 2;


	SDL_Surface* hello_world = NULL;

	GLuint shader_program;

	GLfloat vertex_buffer_[kTotalBufferSize];
	
	int buffer_length_;
	int buffer_count_;

};

CORGI_REGISTER_SYSTEM(SpriteSystem, SpriteData)


#endif // SPRITE_SYSTEM_H