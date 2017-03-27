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
  common_data->texture_manager = &texture_manager_;
  common_data->keyboard_input = &keyboard_input_;
}


void MainState::Init() {
  printf("hello world!\n");
  corgi::Entity entity = entity_manager_.AllocateNewEntity();
  entity_manager_.RegisterSystem(&asteroid_system_);
	entity_manager_.RegisterSystem(&common_system_);
	entity_manager_.RegisterSystem(&transform_system_);
	entity_manager_.RegisterSystem(&sprite_system_);
  entity_manager_.RegisterSystem(&physics_system_);
  entity_manager_.RegisterSystem(&wallbounce_system_);
  entity_manager_.RegisterSystem(&player_ship_system_);
  entity_manager_.RegisterSystem(&fade_timer_system_);
  entity_manager_.RegisterSystem(&bullet_system_);

	entity_manager_.set_max_worker_threads(0);

  entity_manager_.FinalizeSystemList();

	corgi::Entity spritetest;
	SpriteData* sprite_data;
	TransformData* transform_data;

  corgi::Entity player_ship = entity_manager_.AllocateNewEntity();
  entity_manager_.AddComponent<PlayerShip>(player_ship);
}


void MainState::Render(double delta_time) {
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
  glClearDepth(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sprite_system_.RenderSprites();
	SDL_GL_SwapWindow(common_system_.CommonData()->window);
}


void MainState::UpdateInput() {
  keyboard_input_.ClearForUpdate();
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      printf("Exit signal detected\n");
      EndState();
      break;
    case SDL_KEYDOWN:
      keyboard_input_.SetKeyState(event.key.keysym.sym, true);
      break;
    case SDL_KEYUP:
      keyboard_input_.SetKeyState(event.key.keysym.sym, false);
      break;
    default:
      break;
    }
  }
}


void MainState::Update(double delta_time) {
  entity_manager_.UpdateSystems(delta_time);
  UpdateInput();
  if (keyboard_input_.GetKeyState(SDLK_ESCAPE).was_released) {
    EndState();
  }

}
