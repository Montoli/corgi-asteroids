#include "main_state.h"
#include <SDL.h>
#include <stdio.h>

void MainState::Init() {
  printf("hello world!\n");
  corgi::EntityRef entity = entity_manager_.AllocateNewEntity();
  entity_manager_.RegisterComponent(&test_system_);
  test_system_.AddEntity(entity);
  //entity_manager_.AddEntityToComponent(entity);
}


void MainState::Update(float delta_time) {

  entity_manager_.UpdateComponents(delta_time);

  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    /* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
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
