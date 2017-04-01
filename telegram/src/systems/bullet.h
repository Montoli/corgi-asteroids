#ifndef BULLET_H
#define BULLET_H
#include "corgi/system.h"
#include "math_common.h"
#include <unordered_set>
#include "constants.h"

struct BulletData {
};

const float kBucketSize = 20.0f;

const int kBucketColumns = 1 + (int)(kScreenWidth / 20.0f);
const int kBucketRows = 1 + (int)(kScreenHeight / 20.0f);

const float kBulletRadius = 2.5f;
const float kBulletDamage = 2.0f;


class BulletSystem : public corgi::System<BulletData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);
  virtual void DeclareDependencies();
  virtual void InitEntity(corgi::Entity entity);
  virtual void CleanupEntity(corgi::Entity entity);

  // Checks to see if an asteroid has been hit.  If so,
  // returns the bullet that hit it.
  void CheckForAsteroidHit(corgi::Entity asteroid);

  void SpawnHitSparks(corgi::Entity bullet);

private:
  std::unordered_set<corgi::Entity> collision_map[kBucketRows * kBucketColumns];
  int BulletSystem::GetBucketIndex(vec2 position);

};

CORGI_REGISTER_SYSTEM(BulletSystem, BulletData)


#endif // BULLET_H