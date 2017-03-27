#ifndef BULLET_H
#define BULLET_H
#include "corgi/system.h"
#include "math_common.h"

struct BulletData {
};

class BulletSystem : public corgi::System<BulletData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);
  virtual void DeclareDependencies();
  virtual void InitEntity(corgi::Entity entity);

private:
  void SpawnHitSparks(corgi::Entity bullet);
};

CORGI_REGISTER_SYSTEM(BulletSystem, BulletData)


#endif // BULLET_H