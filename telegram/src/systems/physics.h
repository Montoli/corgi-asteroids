#ifndef PHYSICS_H
#define PHYSICS_H
#include "corgi/system.h"
#include "math_common.h"

struct PhysicsData {
	PhysicsData()
		: velocity(vec2(0, 0)),
		acceleration(vec2(0, 0)),
		angular_velocity(quat::identity),
		angular_acceleration(quat::identity) {}

	vec2 velocity;
	vec2 acceleration;
	quat angular_velocity;
	quat angular_acceleration;
};


class PhysicsSystem : public corgi::System<PhysicsData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();

};

CORGI_REGISTER_SYSTEM(PhysicsSystem, PhysicsData)


#endif // PHYSICS_H