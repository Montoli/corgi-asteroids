#include <SDL.h>
#include "sprite.h"
#include "transform.h"

CORGI_DEFINE_SYSTEM(SpriteSystem, SpriteComponent)
/*
void SpriteSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
	//printf("entity updated!");
  for (auto itr = begin(); itr != end(); ++itr) {
    corgi::EntityRef entity = itr->entity;
		// do stuff
  }
}
*/
void SpriteSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
	printf("SpriteSystem - starting update!\n");


	SDL_Delay(500);
	printf("SpriteSystem - ending update!\n");

}



void SpriteSystem::DeclareDependencies() {
	// don't actually need this unless we're doing preprocessing.
  DependOn<TransformSystem>(corgi::kExecuteAfter, corgi::kReadAccess);
}


void SpriteSystem::InitEntity(corgi::EntityRef& entity) {

}

void SpriteSystem::Init() {


	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;

	//The surface contained by the window
	SDL_Surface* gScreenSurface = NULL;

	//The image we will load and show on the screen
	SDL_Surface* gHelloWorld = NULL;

	gHelloWorld = SDL_LoadBMP("02_getting_an_image_on_the_screen/hello_world.bmp");


}