#ifndef WALLBOUNCE_H
#define WALLBOUNCE_H
#include "corgi/system.h"
#include "math_common.h"

struct WallBounceData {
};


class WallBounceSystem : public corgi::System<WallBounceData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();
};

CORGI_REGISTER_SYSTEM(WallBounceSystem, WallBounceData)


#endif // WALLBOUNCE_H