#pragma once
#include <iostream>
#include "ecs_manager.h"

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
		void operator()(Comp1&, Comp2&) const noexcept { printType(*this);  }
	};

	struct Sys2 : public ecs::system::base 
	{
		void operator()(Comp1&, Comp2&) const noexcept { printType(*this); }
	};

	static ecs::manager manager;

	void ecs()
	{
		manager.RegisterComponents<Comp1, Comp2>();
		manager.RegisterSystems<Sys1, Sys2>();
		auto& archetype = manager.GetArchetype<Comp1, Comp2>();
		
		for (int i = 0; i < 100; ++i)
		{
			manager.CreateEntity([&i](Comp1&, Comp2&) { std::cout << "Creating entity " << i << std::endl; }, archetype);
		}

		 manager.Run();

		std::cout << typeid(ecs::component::entity).name() << " : " << ecs::component::info_v<ecs::component::entity>.mUID << '\n';
		std::cout << typeid(Comp1).name() << " : " << ecs::component::info_v<Comp1>.mUID << '\n';
		std::cout << typeid(Comp2).name() << " : " << ecs::component::info_v<Comp2>.mUID << '\n';
	}
}