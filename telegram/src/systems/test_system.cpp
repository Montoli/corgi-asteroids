#include "test_system.h"
#include <stdio.h>

CORGI_DEFINE_COMPONENT(TestSystem, TestData)

void TestSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
  printf("entity updated!");

}