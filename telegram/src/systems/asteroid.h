#ifndef ASTEROID_H
#define ASTEROID_H
#include "corgi/system.h"
#include "math_common.h"

const float kHpScale = 1.0f;
const float kBaseAsteroidSize = 75.0f;

struct AsteroidData {
  float radius = kBaseAsteroidSize;
  float hp = kBaseAsteroidSize * kHpScale;
};

class AsteroidSystem : public corgi::System<AsteroidData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);
  virtual void DeclareDependencies();
  virtual void InitEntity(corgi::Entity entity);

  // Check to see if something has hit an asteroid.  If so,
  // return the asteroid entity hit.
  //corgi::Entity CollisionCheck(vec2 position, float radius);
  // Apply damage to an asteroid.  May blow it up.
  void ApplyDamage(corgi::Entity, float damage);
};

CORGI_REGISTER_SYSTEM(AsteroidSystem, AsteroidData)


#endif // ASTEROID_H