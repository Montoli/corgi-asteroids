#ifndef COMMON_H
#define COMMON_H
#include "corgi/system.h"
#include "math_common.h"
#include "texture_manager.h"
#include "keyboard_input.h"
#include <SDL.h>

struct CommonComponent {
	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr;
	SDL_GLContext gl_context = nullptr;
	vec2 screen_size;
  TextureManager* texture_manager;
  KeyboardInput* keyboard_input;
};

class CommonSystem : public corgi::System<CommonComponent> {
public:

	CommonComponent* CommonData() {
		return &common_data_;
	}

private:
	CommonComponent common_data_;
};

CORGI_REGISTER_SYSTEM(CommonSystem, CommonComponent)

#endif // COMMON_H