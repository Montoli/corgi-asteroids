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

	entity_manager_.set_max_worker_threads(0);

  entity_manager_.FinalizeSystemList();

	corgi::Entity spritetest;
	SpriteData* sprite_data;
	TransformData* transform_data;

  /*
	spritetest = entity_manager_.AllocateNewEntity();
	entity_manager_.AddComponent<SpriteSystem>(spritetest);
	sprite_data = entity_manager_.GetComponentData<SpriteData>(spritetest);
	transform_data = entity_manager_.GetComponentData<TransformData>(spritetest);

	sprite_data->tint = vec4(1.0f, 0.5f, 0.25f, 1.0f);
	transform_data->position = vec3(200.0f, 200.0f, 0.0f);
	sprite_data->size = vec2(100, 50);
	transform_data->scale = vec2(2.0f, 2.0f);
	//------
	spritetest = entity_manager_.AllocateNewEntity();
	entity_manager_.AddComponent<SpriteSystem>(spritetest);
	sprite_data = entity_manager_.GetComponentData<SpriteData>(spritetest);
	transform_data = entity_manager_.GetComponentData<TransformData>(spritetest);

	sprite_data->tint = vec4(0.5f, 1.0f, 0.25f, 1.0f);
	transform_data->position = vec3(300.0f, 50.0f, 0.0f);
	sprite_data->size = vec2(40, 60);
	transform_data->scale = vec2(1.0f, 1.0f);
	transform_data->orientation = quat::FromAngleAxis(2.3f, vec3(0, 0, 1));
  */

  corgi::Entity player_ship = entity_manager_.AllocateNewEntity();
  entity_manager_.AddComponent<PlayerShip>(player_ship);



  for (int i = 0; i < 1; i++) {
    corgi::Entity new_asteroid = entity_manager_.AllocateNewEntity();
    entity_manager_.AddComponent<AsteroidSystem>(new_asteroid);
  }

}


void MainState::Render(double delta_time) {
	glClearColor(0, 1, 0, 1);
  glClearDepth(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sprite_system_.RenderSprites();
	SDL_GL_SwapWindow(common_system_.CommonData()->window);
	//SDL_UpdateWindowSurface(common_system_.CommonData()->window);
}


void MainState::UpdateInput() {
  keyboard_input_.ClearForUpdate();
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // We are only worried about SDL_KEYDOWN and SDL_KEYUP events
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
      //EndState();
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
