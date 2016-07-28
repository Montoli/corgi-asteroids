#include <SDL.h>
#include "physics.h"
#include "transform.h"

CORGI_DEFINE_SYSTEM(PhysicsSystem, PhysicsData)


void PhysicsSystem::DeclareDependencies() {
	DependOn<TransformSystem>(corgi::kExecuteBefore, corgi::kReadWriteAccess);
	SetIsThreadSafe(true);
}



void PhysicsSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
	for (auto itr = begin(); itr != end(); ++itr) {
		corgi::Entity entity = itr->entity;
		TransformData* transform_data = Data<TransformData>(entity);
		PhysicsData* physics_data = Data<PhysicsData>(entity);
		transform_data->position += vec3(physics_data->velocity.x(),
					physics_data->velocity.y(), 0);
		physics_data->velocity += physics_data->acceleration;
		//todo - add a max velocity here?

		transform_data->orientation =
  			transform_data->orientation * physics_data->angular_velocity;
		physics_data->angular_velocity =
			  physics_data->angular_velocity * physics_data->angular_acceleration;
	}
}
