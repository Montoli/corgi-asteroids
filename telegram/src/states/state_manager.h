#ifndef STATEMANAGER_H
#define STATEMANAGER_H
#include <memory>
#include "corgi\entity_manager.h"
#include "base_state.h"

class StateManager {
public:
  void Update(double delta_time);
  void Render(double delta_time);

   void PushState(BaseState* new_state);

  BaseState * GetTopState();
  int PopEndedStates();

  int StateCount() { return state_stack_.size(); }
  bool IsAppQuitting() { return is_app_quitting_;  }
  BaseState* StateManager::GetCurrentlyExecutingState() { return currently_executing_state_; }


private:
  void EndOfUpdateCleanup();

  corgi::EntityManager entity_manager;

  std::vector<std::unique_ptr<BaseState>> state_stack_;
  std::vector<std::unique_ptr<BaseState>> states_to_push_;

  bool is_app_quitting_ = false;
  bool stack_locked_ = false;

  // Normally the current state is the top of the stack, but sometimes
  // this can change - if a state needs to invoke some or all of its parent,
  // for example.
  BaseState* currently_executing_state_ = nullptr;
};



#endif // STATEMANAGER_H