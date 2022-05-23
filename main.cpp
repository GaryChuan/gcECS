#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include <iostream>
#include "ecs/ecs_manager.h"

struct A{};

int main()
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

	ecs::manager manager;
	ecs::archetype archetype;

	manager.RegisterComponents<A>();

	archetype.Initialize(std::array{ &ecs::component::info_v<ecs::component::entity>, 
									 & ecs::component::info_v<A>});

	for (int i = 0; i < 100; ++i)
	{
		archetype.CreateEntity();
	}

	std::cout << typeid(ecs::component::entity).name() << " : " << ecs::component::info_v<ecs::component::entity>.mUID << '\n';
	std::cout << typeid(A).name() << " : " << ecs::component::info_v<A>.mUID << '\n';
}