#ifndef ASTEROID_H
#define ASTEROID_H
#include "corgi/system.h"
#include "math_common.h"

struct AsteroidData {
  float radius = 50;
};


class AsteroidSystem : public corgi::System<AsteroidData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();

  virtual void InitEntity(corgi::Entity entity);

};

CORGI_REGISTER_SYSTEM(AsteroidSystem, AsteroidData)


#endif // ASTEROID_H