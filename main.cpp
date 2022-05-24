#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "tests/test_ecs.h"
#include "tests/test_traits_function.h"

struct base {};

template <typename T>
void printType(const T&)
{
	std::cout << typeid(T).name() << '\n';
}

struct sys1 : base 
{
	void operator()(int, int) { printType(*this); };
};
struct sys2 : base
{
	void operator()(float, float) { printType(*this); };
};

template <typename T>
struct interface : T
{
	void Run()
	{
		std::cout << typeid(args).name() << '\n';
	}

	core::function::traits<T>::argument_types args;
};

template <typename Ret, typename... Args>
struct tBase
{
	using type = Ret(Args...);
};

template <typename TFunc>
struct tDerived;

template <typename Ret, typename... Args>
struct tDerived<Ret(*)(Args...)> : tBase<Ret, Args...>
{

};

int main()
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

	std::unique_ptr<base> p2 = std::make_unique<interface<sys1>>();

	static_cast<interface<sys1>&>(*p2).Run();

	// tDerived<decltype(&test)>

	// test::ecs();
	test::function_traits();
}