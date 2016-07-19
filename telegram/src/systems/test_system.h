#ifndef TEST_SYSTEM_H
#define TEST_SYSTEM_H
#include "corgi/system.h"

struct TestData {
  int num = 42;
};

class TestSystem : public corgi::System<TestData> {
public:

virtual void UpdateAllEntities(corgi::WorldTime delta_time);

virtual void InitEntity(corgi::EntityRef& entity);

};

CORGI_REGISTER_SYSTEM(TestSystem, TestData)


#endif // TEST_SYSTEM_H