#ifndef MAINSTATE_H
#define MAINSTATE_H
#include <memory>
#include "corgi/entity_manager.h"
#include "systems/test_system.h"
#include "systems/test_system2.h"

#include "systems/transform.h"
#include "systems/sprite.h"
#include "systems/common.h"

#include "base_state.h"

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

private:
  corgi::EntityManager entity_manager_;
  TestSystem test_system_;
  TestSystem2 test_system2_;

	CommonSystem common_system_;
	SpriteSystem sprite_system_;
	TransformSystem transform_system_;



};



#endif // MAINSTATE_H