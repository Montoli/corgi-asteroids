#include <SDL.h>
#include "transform.h"

CORGI_DEFINE_SYSTEM(TransformSystem, TransformData)


void TransformSystem::DeclareDependencies() {
}


void TransformSystem::Init() {
	SetIsThreadSafe(true);
}


void TransformSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
	printf("TransformSystem - starting update!\n");


	// illegal without declaring accesss dependency...

	/*
	for (auto itr = begin(); itr != end(); ++itr) {
	corgi::EntityRef entity = itr->entity;
	TestData2* testdata = Data<TestData2>(entity);
	}
	*/
	SDL_Delay(500);
	printf("TransformSystem - ending update!\n");

}
