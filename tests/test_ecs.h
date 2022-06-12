#pragma once
#include <iostream>
#include "../ecs/ecs_manager.h"

namespace test
{
	struct Comp1 
	{
		static constexpr auto typedef_v = ecs::component::type::data
		{
			.mName = "Comp1"
		};
	};

	struct Comp2 
	{
		static constexpr auto typedef_v = ecs::component::type::data
		{
			.mName = "Comp2"
		};
	};

	template <typename T>
	void printType(const T&)
	{
		std::cout << typeid(T).name() << '\n';
	}

	struct Sys1 : public ecs::system::base 
	{
		void operator()(Comp1&, Comp1&) const noexcept { printType(*this);  }
	};

	struct Sys2 : public ecs::system::base 
	{
		void operator()(Comp2&, Comp2&) const noexcept { printType(*this); }
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