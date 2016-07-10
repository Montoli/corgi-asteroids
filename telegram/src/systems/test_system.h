#ifndef TEST_SYSTEM_H
#define TEST_SYSTEM_H
#include "corgi/component.h"

struct TestData {
  int num = 42;
};


class TestSystem : public corgi::Component<TestData> {
public:

virtual void UpdateAllEntities(corgi::WorldTime delta_time);
};

CORGI_REGISTER_COMPONENT(TestData, TestSystem)


#endif // TEST_SYSTEM_H