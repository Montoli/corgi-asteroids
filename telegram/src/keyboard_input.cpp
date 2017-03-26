#include "keyboard_input.h"


void KeyboardInput::SetKeyState(SDL_Keycode key, bool isDown) {
  if (key_state.find(key) == key_state.end()) {
    key_state[key] = KeyState();
  }

  KeyState& currentKey = key_state[key];

  if (isDown) {
    currentKey.was_pressed = !currentKey.is_down;
    currentKey.was_released = false;
  } else {
    currentKey.was_pressed = false;
    currentKey.was_released = currentKey.is_down;
  }

  currentKey.is_down = isDown;
}

void KeyboardInput::ClearForUpdate() {
  for (auto itr = key_state.begin(); itr != key_state.end(); ++itr) {
    itr->second.was_pressed = false;
    itr->second.was_released = false;
  }
}

KeyState KeyboardInput::GetKeyState(SDL_Keycode key) {
  if (key_state.find(key) == key_state.end()) {
    key_state[key] = KeyState();
  }
  return key_state[key];
}