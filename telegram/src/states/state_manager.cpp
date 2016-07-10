#include "state_manager.h"

/*
class StateManager {
public:
void Update(float delta_time);
void Render(float delta_time);


int StateCount() { return state_stack_.size(); }
bool IsAppQuitting() { return is_app_quitting_;  }

private:

corgi::EntityManager entity_manager;

std::vector<std::unique_ptr<BaseState>> state_stack_;
std::vector<std::unique_ptr<BaseState>> states_to_push_;

bool is_app_quitting_ = false;
bool stack_locked_ = false;
};

*/

void StateManager::Update(float delta_time) {
  if (state_stack_.size() > 0) {
    // Store off the old current state, and update it.  Then call the update function
    BaseState* old_current = currently_executing_state_;
    currently_executing_state_ = state_stack_[state_stack_.size() - 1].get();
    currently_executing_state_->Update(delta_time);

    // set the current back to whatever it was before we did this...
    currently_executing_state_ = old_current;
  }
  else {
    if (states_to_push_.size() == 0) {
      // no states, so we might as well quit.
      is_app_quitting_ = true;
    }
  }
  EndOfUpdateCleanup();
}

void StateManager::Render(float delta_time) {
  // todo: write this.  Should be basically he same as update.
}


int StateManager::PopEndedStates() {
  int states_removed_count = 0;
  // Remove everything (that's uncovered) that needs removing
  while (state_stack_.size() > 0 &&
    state_stack_[state_stack_.size() - 1]->IsEnding()) {
    state_stack_[state_stack_.size() - 1]->Cleanup();
    state_stack_.pop_back();
    states_removed_count++;

    // Let whatever was below know that it's time to wake up.
    // (It might be about to end itself, but still, it gets
    // a resume call.)
    if (state_stack_.size() > 0)
      state_stack_[state_stack_.size() - 1]->Resume();
  }

  return states_removed_count;
}


void StateManager::EndOfUpdateCleanup() {
  // clean up the stack, remove any states that ended.
  PopEndedStates();

  // push any pending stacks on to the stack
  for (auto itr = states_to_push_.begin(); itr != states_to_push_.end(); ++itr) {
    state_stack_.push_back(std::move(*itr));
    // need to call startup here
  }
  states_to_push_.clear();
}

// pushing does not actually occur until the end of the update phase.  (States that are
// pushed won't execute until next frame.)
void StateManager::PushState(BaseState* new_state) {
  // First remove any states that are floating around and waiting to end...
  PopEndedStates();

  // Suspend the current top, if any...
  BaseState* current_state = GetTopState();
  if (current_state != nullptr) current_state->Suspend();

  // push the new state and init it.
  new_state->SetStateManager(this);
  states_to_push_.push_back(std::unique_ptr<BaseState>(new_state));
  new_state->Init();
}

BaseState* StateManager::GetTopState() {
  return state_stack_.size() > 0 ? state_stack_[state_stack_.size() - 1].get() : nullptr;
}


