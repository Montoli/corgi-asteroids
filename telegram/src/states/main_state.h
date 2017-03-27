#ifndef MAINSTATE_H
#define MAINSTATE_H
#include <memory>
#include "corgi/entity_manager.h"

#include "systems/asteroid.h"
#include "systems/transform.h"
#include "systems/sprite.h"
#include "systems/common.h"
#include "systems/physics.h"
#include "systems/wallbounce.h"
#include "systems/playership.h"
#include "systems/fade_timer.h"
#include "systems/bullet.h"

#include "base_state.h"
#include "keyboard_input.h"
#include "texture_manager.h"


class MainState : public BaseState {
public:
	MainState(SDL_Window* window, SDL_Surface* screen_surface,
		SDL_GLContext context, int screen_width, int screen_height);


	virtual void Update(double delta_time);
	virtual void Render(double delta_time);

  virtual void Init();
  virtual void Cleanup() {}

  virtual void Suspend() {}
  virtual void Resume() {}

  void UpdateInput();

private:
  KeyboardInput keyboard_input_;
  TextureManager texture_manager_;


  corgi::EntityManager entity_manager_;

  AsteroidSystem asteroid_system_;
  PlayerShip player_ship_system_;
  CommonSystem common_system_;
	SpriteSystem sprite_system_;
	TransformSystem transform_system_;
  PhysicsSystem physics_system_;
  WallBounceSystem wallbounce_system_;
  FadeTimerSystem fade_timer_system_;
  BulletSystem bullet_system_;


};



#endif // MAINSTATE_H