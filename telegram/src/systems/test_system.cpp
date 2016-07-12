#include "test_system.h"
#include "test_system2.h"
#include <stdio.h>

CORGI_DEFINE_SYSTEM(TestSystem, TestData)

void TestSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
  //  printf("entity updated!");

  
  for (auto itr = begin(); itr != end(); ++itr) {
    corgi::EntityRef entity = itr->entity;
    TestData2* testdata = Data<TestData2>(entity);
  }

}


void TestSystem::InitEntity(corgi::EntityRef& entity) {
  printf("entity from system 1 initted!");
}