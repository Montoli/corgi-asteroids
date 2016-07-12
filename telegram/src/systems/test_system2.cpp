#include "test_system2.h"
#include "test_system.h"
#include <stdio.h>

CORGI_DEFINE_SYSTEM(TestSystem2, TestData2)

void TestSystem2::UpdateAllEntities(corgi::WorldTime delta_time) {
  //printf("entity updated!");
  for (auto itr = begin(); itr != end(); ++itr) {
    corgi::EntityRef entity = itr->entity;
    TestData* testdata = Data<TestData>(entity);
  }


}

void TestSystem2::DeclareDependencies() {

  DependOn<TestSystem>(corgi::kMustHaveComponent);
}


void TestSystem2::InitEntity(corgi::EntityRef& entity) {
  printf("entity from system 2 initted!");
}