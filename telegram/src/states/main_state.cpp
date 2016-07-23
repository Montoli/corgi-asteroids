#include "main_state.h"
#include <SDL.h>
#include <stdio.h>


MainState::MainState(SDL_Window* window, SDL_Surface* screen_surface) {
	CommonComponent* common_data = common_system_.CommonData();
	common_data->window = window;
	common_data->screen_surface = screen_surface;
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


void MainState::Update(double delta_time) {
	printf("---------------------------------------------\n");
	printf("start of update!\n");
	printf("---------------------------------------------\n");

  entity_manager_.UpdateSystems(delta_time);

  SDL_Event event;


	sprite_system_.RenderSprites();
	SDL_UpdateWindowSurface(common_system_.CommonData()->window);


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
