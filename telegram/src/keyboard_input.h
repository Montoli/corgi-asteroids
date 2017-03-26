#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <SDL.h>
#include <map>



struct KeyState {
  KeyState() : is_down(false), was_pressed(false), was_released(false) {}
  bool is_down;
  bool was_pressed;
  bool was_released;
};

// Quick and dirty keyboard input manager
class KeyboardInput {
public:

  void ClearForUpdate();

  void SetKeyState(SDL_Keycode key, bool isDown);
  KeyState GetKeyState(SDL_Keycode key);

private:
  std::map<SDL_Keycode, KeyState> key_state;
};


#endif // KEYBOARD_INPUT_H