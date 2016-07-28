#ifndef TEST_FOUNTAIN_PROJETILE_H
#define TEST_FOUNTAIN_PROJETILE_H
#include "corgi/system.h"
#include "math_common.h"

struct FountainData {
	vec2 velocity;
};


class FountainProjectile : public corgi::System<FountainData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();

  virtual void InitEntity(corgi::Entity entity);

};

CORGI_REGISTER_SYSTEM(FountainProjectile, FountainData)


#endif // TEST_FOUNTAIN_PROJETILE_H