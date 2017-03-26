#ifndef PLAYERSHIP_H
#define PLAYERSHIP_H
#include "corgi/system.h"
#include "math_common.h"

struct PlayerShipData {
  int gun_cooldown;
};


class PlayerShip : public corgi::System<PlayerShipData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();

  virtual void InitEntity(corgi::Entity entity);

private:
  void SpawnExhaust(corgi::Entity ship);

};

CORGI_REGISTER_SYSTEM(PlayerShip, PlayerShipData)


#endif // PLAYERSHIP_H