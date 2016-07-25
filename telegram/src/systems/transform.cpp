#include <SDL.h>
#include "transform.h"

CORGI_DEFINE_SYSTEM(TransformSystem, TransformData)


void TransformSystem::DeclareDependencies() {
}


void TransformSystem::Init() {
	SetIsThreadSafe(true);
}


void TransformSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
//	printf("TransformSystem - starting update!\n");

	//printf("TransformSystem - ending update!\n");

}
