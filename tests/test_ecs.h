#pragma once
#include <iostream>
#include "../ecs/ecs_manager.h"

namespace test
{
	struct Comp1 {};
	struct Comp2 {};

	struct Sys1 : ecs::system::base {};
	struct Sys2 : ecs::system::base {};

	void ecs()
	{
		ecs::manager manager;
		ecs::archetype archetype;

		manager.RegisterComponents<Comp1, Comp2>();
		manager.RegisterSystems<Sys1, Sys2>();


		archetype.Initialize(std::array{ &ecs::component::info_v<ecs::component::entity>,
										 & ecs::component::info_v<Comp1>});

		for (int i = 0; i < 100; ++i)
		{
			archetype.CreateEntity();
		}

		std::cout << typeid(ecs::component::entity).name() << " : " << ecs::component::info_v<ecs::component::entity>.mUID << '\n';
		std::cout << typeid(Comp1).name() << " : " << ecs::component::info_v<Comp1>.mUID << '\n';
		std::cout << typeid(Comp2).name() << " : " << ecs::component::info_v<Comp2>.mUID << '\n';
	}
}