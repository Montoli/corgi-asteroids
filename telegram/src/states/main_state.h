#ifndef MAINSTATE_H
#define MAINSTATE_H
#include <memory>
#include "corgi/entity_manager.h"
#include "systems/test_system.h"
#include "base_state.h"

class MainState : public BaseState {
public:

  virtual void Update(float delta_time);
  virtual void Render(float delta_time) {}

  virtual void Init();
  virtual void Cleanup() {}

  virtual void Suspend() {}
  virtual void Resume() {}

private:
  corgi::EntityManager entity_manager_;
  TestSystem test_system_;

};



#endif // MAINSTATE_H