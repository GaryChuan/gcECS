#pragma once
#include <iostream>
#include "../core/functions.h"

namespace test
{
	struct A
	{
		void Base() {};
		void NoExcept() noexcept {};
		void Const() const {};
		void ConstNoExcept() const noexcept {};
	};

	struct B
	{
		void operator ()(int) {}
	};

	struct C
	{
	};

	void Basic() {};
	void NoExcept() noexcept {};

	void function_traits()
	{
		// Public functions
		std::cout << typeid(core::function::traits<decltype(Basic)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(NoExcept)>::signature).name() << '\n';

		// Public function pointers
		std::cout << typeid(core::function::traits<decltype(&Basic)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&NoExcept)>::signature).name() << '\n';

		// Member function pointers
		std::cout << typeid(core::function::traits<decltype(&A::Base)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&A::NoExcept)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&A::Const)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&A::ConstNoExcept)>::signature).name() << '\n';

		// Testing functors
		B functor;
		const B constFunctor;

		B& refFunctor = functor;
		const B& crefFunctor = constFunctor;

		B b1, b2;
		B&& rrefFunctor = std::move(b1);
		const B&& crrefFunctor = std::move(b2);

		B* pFunctor = &functor;
		const B* cpFunctor = &functor;

		std::cout << typeid(core::function::traits<B>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&functor)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&constFunctor)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&refFunctor)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&crefFunctor)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&rrefFunctor)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(&crrefFunctor)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(pFunctor)>::signature).name() << '\n';
		std::cout << typeid(core::function::traits<decltype(cpFunctor)>::signature).name() << '\n';
	}
}