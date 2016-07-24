/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

#include "GL/glew.h"

#include "states/state_manager.h"
#include "states/main_state.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[])
{
  //The window we'll be rendering to
  SDL_Window* window = NULL;

  //The surface contained by the window
  SDL_Surface* screen_surface = NULL;

	// init Glew
	glewInit();

  //Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  }
  else
  {
    //Create window
    window = SDL_CreateWindow(
			"SDL Tutorial", 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			SCREEN_WIDTH,
			SCREEN_HEIGHT, 
			SDL_WINDOW_OPENGL);

    if (window == NULL)
    {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {

			SDL_GLContext gl_context = SDL_GL_CreateContext(window);


			// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
			//SDL_GL_DeleteContext(glcontext);

			// Set our OpenGL version.
			// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

			// 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

			// Turn on double buffering with a 24bit Z buffer.
			// You may need to change this to 16 or 32 for your system
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

			// This makes our buffer swap syncronized with the monitor's vertical refresh
			SDL_GL_SetSwapInterval(1);





      //Get window surface
			//screen_surface = SDL_GetWindowSurface(window);

      //Fill the surface white
      //SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF));

      //Update the surface
      SDL_UpdateWindowSurface(window);


			// now you can make GL calls.
			glClearColor(0, 1, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			SDL_GL_SwapWindow(window);


      StateManager state_manager;

      state_manager.PushState(new MainState(window, screen_surface));

      while (!state_manager.IsAppQuitting()) {
        SDL_Delay(1000/60);
        state_manager.Update(1000 / 60);
      }
			SDL_GL_DeleteContext(gl_context);
		}
  }

  //Destroy window
	SDL_DestroyWindow(window);

  //Quit SDL subsystems
  SDL_Quit();

  return 0;
}