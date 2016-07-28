#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "corgi/system.h"
#include "math_common.h"

struct TransformData {
	TransformData()
		: origin(vec2(0, 0)),
		position(vec3(0, 0, 0)),
		scale(vec2(1, 1)),
		orientation(quat::identity) {}

	vec2 origin;
	vec3 position;
	vec2 scale;
	quat orientation;

	mat4 GetTransformMatrix() {
		// Start with origin transform:

		mat3 rot = orientation.ToMatrix();

		// Break it up into columns:
		vec4 c0 = vec4(rot[0], rot[3], rot[6], 0);
		vec4 c1 = vec4(rot[1], rot[4], rot[7], 0);
		vec4 c2 = vec4(rot[2], rot[5], rot[8], 0);
		vec4 c3 = vec4(0, 0, 0, 1);
		// Apply scale:
		c0 *= scale.x();
		c1 *= scale.y();
		//c2 *= scale.z();

		
		// Apply translation:
		c3[0] = position.x();
		c3[1] = position.y();
		c3[2] = position.z();
		
		// Compose and return result:
		return mat4(c0, c1, c2, c3);
	}
};


class TransformSystem : public corgi::System<TransformData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();

	//virtual void InitEntity(corgi::Entity entity);
	virtual void Init();

};

CORGI_REGISTER_SYSTEM(TransformData, TransformSystem)


#endif // TRANSFORM_H