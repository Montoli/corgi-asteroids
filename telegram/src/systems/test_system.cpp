#include "test_system.h"
#include "test_system2.h"
#include <stdio.h>
#include <SDL.h>

CORGI_DEFINE_SYSTEM(TestSystem, TestData)

void TestSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
//    printf("Test System 1 - starting update!\n");


  // illegal without declaring accesss dependency...
  
  /*
  for (auto itr = begin(); itr != end(); ++itr) {
    corgi::EntityRef entity = itr->entity;
    TestData2* testdata = Data<TestData2>(entity);
  }
  */
//		SDL_Delay(500);
		//printf("Test System 1 - ending update!\n");

}


void TestSystem::InitEntity(corgi::EntityRef& entity) {
  printf("entity from system 1 initted!");
	SetIsThreadSafe(true);

}