#include "main_state.h"
#include <SDL.h>
#include <stdio.h>
#include "GL/glew.h"


MainState::MainState(SDL_Window* window, SDL_Surface* screen_surface,
	SDL_GLContext context, int screen_width, int screen_height) {
	CommonComponent* common_data = common_system_.CommonData();
	common_data->window = window;
	common_data->screen_surface = screen_surface;
	common_data->gl_context = context;
	common_data->screen_size = vec2(static_cast<float>(screen_width),
														static_cast<float>(screen_height));
}


void MainState::Init() {
  printf("hello world!\n");
  corgi::EntityRef entity = entity_manager_.AllocateNewEntity();
	entity_manager_.RegisterSystem(&common_system_);
	entity_manager_.RegisterSystem(&test_system_);
  entity_manager_.RegisterSystem(&test_system2_);
	entity_manager_.RegisterSystem(&transform_system_);
	entity_manager_.RegisterSystem(&sprite_system_);

  entity_manager_.FinalizeSystemList();

  test_system2_.AddEntity(entity);
  //entity_manager_.AddEntityToSystem(entity);
}


void MainState::Render(double delta_time) {
	glClearColor(0, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	sprite_system_.RenderSprites();
	SDL_GL_SwapWindow(common_system_.CommonData()->window);
	//SDL_UpdateWindowSurface(common_system_.CommonData()->window);
}


void MainState::Update(double delta_time) {
	printf("---------------------------------------------\n");
	printf("start of update!\n");
	printf("---------------------------------------------\n");

  entity_manager_.UpdateSystems(delta_time);

  SDL_Event event;

	

  while (SDL_PollEvent(&event)) {
    // We are only worried about SDL_KEYDOWN and SDL_KEYUP events
    switch (event.type) {
    case SDL_KEYDOWN:
      printf("Key press detected\n");
      break;

    case SDL_KEYUP:
      printf("Key release detected\n");
      EndState();
      break;

    default:
      break;
    }
  }
}
