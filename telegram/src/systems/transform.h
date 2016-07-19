#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "corgi/system.h"

struct TransformData {
	float x, y;
	float scale_x, scale_y;
	float rotation;
};


class TransformSystem : public corgi::System<TransformData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();

	//virtual void InitEntity(corgi::EntityRef& entity);
	virtual void Init();

};

CORGI_REGISTER_SYSTEM(TransformData, TransformSystem)


#endif // TRANSFORM_H