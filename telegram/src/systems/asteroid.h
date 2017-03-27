#ifndef ASTEROID_H
#define ASTEROID_H
#include "corgi/system.h"
#include "math_common.h"

const float kHpScale = 1.0f;

struct AsteroidData {
  AsteroidData() : radius(50), hp(50) {}
  AsteroidData(float rad) {
    radius = rad;
    hp = rad * kHpScale;
  }

  float radius = 50;
  float hp = 50;
};

class AsteroidSystem : public corgi::System<AsteroidData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);
  virtual void DeclareDependencies();
  virtual void InitEntity(corgi::Entity entity);
};

CORGI_REGISTER_SYSTEM(AsteroidSystem, AsteroidData)


#endif // ASTEROID_H