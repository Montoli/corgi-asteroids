#include "sprite.h"
#include "transform.h"
#include "common.h"
#include "GL/glew.h"

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
  DependOn<TransformSystem>(corgi::kExecuteAfter, corgi::kReadAccess);
	DependOn<CommonSystem>(corgi::kNoOrderDependency, corgi::kReadAccess);
}


void SpriteSystem::Cleanup() {
	//Deallocate surface
	SDL_FreeSurface(hello_world);
	hello_world = NULL;

}

void SpriteSystem::InitEntity(corgi::EntityRef& entity) {

}

void SpriteSystem::RenderSprites() {
//	SDL_BlitSurface(hello_world, NULL, screen_surface, NULL);

	//glClearColor(0, 0, 0, 1);
	//glClear(GL_COLOR_BUFFER_BIT);
	//SDL_GL_SwapWindow(window);


}

SDL_Surface* SpriteSystem::LoadPNG(std::string path) {
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;
	CommonComponent* common = entity_manager_->GetSystem<CommonSystem>()->CommonData();
	/*
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, common->screen_surface->format, NULL);
		if (optimizedSurface == NULL) {
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	*/
	return optimizedSurface;
}



void SpriteSystem::Init() {

	/*
	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;

	//The surface contained by the window
	SDL_Surface* gScreenSurface = NULL;

	//The image we will load and show on the screen
	*/

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		assert(false);
	}

	/*

	//hello_world = SDL_LoadBMP("rsc\\hello_world.bmp");
	hello_world = LoadPNG("rsc/welshcorgi.png");

	if (hello_world == NULL) {
		printf("Unable to load image! SDL Error: %s\n",  SDL_GetError());
		assert(false);
	}
	*/

}