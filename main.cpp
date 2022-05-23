#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include <iostream>
#include "ecs/ecs_manager.h"
#include "core/function_traits.h"

struct A
{
	void Base() {};
	void NoExcept() noexcept {};
	void Const() const {};
	void ConstNoExcept() const noexcept {};
};

struct B
{
	void operator ()(int){}
};

void Basic() {};
void NoExcept() noexcept {};

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

	std::cout << typeid(core::traits::function<decltype(Basic)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(NoExcept)>::type).name() << '\n';

	std::cout << typeid(core::traits::function<decltype(&Basic)>::type).name() <<'\n';
	std::cout << typeid(core::traits::function<decltype(&NoExcept)>::type).name() << '\n';

	std::cout << typeid(core::traits::function<decltype(&A::Base)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(&A::NoExcept)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(&A::Const)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(&A::ConstNoExcept)>::type).name() << '\n';
	
	B functor;
	const B constFunctor;

	B& refFunctor = functor;
	const B& crefFunctor = constFunctor;

	B b1, b2;
	B&& rrefFunctor = std::move(b1);
	const B&& crrefFunctor = std::move(b2);

	B* pFunctor = &functor;
	const B* cpFunctor = &functor;

	std::cout << typeid(core::traits::function<B>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(&functor)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(&constFunctor)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(&refFunctor)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(&crefFunctor)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(&rrefFunctor)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(&crrefFunctor)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(pFunctor)>::type).name() << '\n';
	std::cout << typeid(core::traits::function<decltype(cpFunctor)>::type).name() << '\n';

	std::cout << typeid(ecs::component::entity).name() << " : " << ecs::component::info_v<ecs::component::entity>.mUID << '\n';
	std::cout << typeid(A).name() << " : " << ecs::component::info_v<A>.mUID << '\n';
}