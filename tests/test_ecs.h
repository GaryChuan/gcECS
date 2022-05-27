#pragma once
#include <iostream>
#include "../ecs/ecs_manager.h"

namespace test
{
	struct Comp1 : ecs::component::base {};
	struct Comp2 : ecs::component::base {};

	template <typename T>
	void printType(const T&)
	{
		std::cout << typeid(T).name() << '\n';
	}

	struct Sys1 : public ecs::system::base 
	{
		void operator()(float, float) { printType(*this);  }
	};

	struct Sys2 : public ecs::system::base 
	{
		void operator()(int, int) { printType(*this); }
	};

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

		manager.Run();

		std::cout << typeid(ecs::component::entity).name() << " : " << ecs::component::info_v<ecs::component::entity>.mUID << '\n';
		std::cout << typeid(Comp1).name() << " : " << ecs::component::info_v<Comp1>.mUID << '\n';
		std::cout << typeid(Comp2).name() << " : " << ecs::component::info_v<Comp2>.mUID << '\n';
	}
}