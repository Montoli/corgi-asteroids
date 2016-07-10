#include "test_system.h"
#include <stdio.h>

CORGI_DEFINE_SYSTEM(TestSystem, TestData)

void TestSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
//  printf("entity updated!");

}


void TestSystem::InitEntity(corgi::EntityRef& entity) {
  printf("entity from system 1 initted!");
}