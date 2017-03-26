#ifndef FADE_TIMER_H
#define FADE_TIMER_H
#include "corgi/system.h"

struct FadeTimerData {
  corgi::WorldTime counter;
  corgi::WorldTime fade_point;
};


class FadeTimerSystem : public corgi::System<FadeTimerData> {
public:

  virtual void UpdateAllEntities(corgi::WorldTime delta_time);

  virtual void DeclareDependencies();
};

CORGI_REGISTER_SYSTEM(FadeTimerSystem, FadeTimerData)


#endif // FADE_TIMER_H