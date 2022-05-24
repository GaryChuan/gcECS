#pragma once
#include <tuple>
#include <concepts>

namespace core::function
{
	template <bool NoExcept, typename Ret, typename... Args>
	requires ( std::is_same_v<void, Ret> )
	struct traits_base
	{
		using signature		 = Ret(Args...) noexcept(NoExcept);
		using class_type	 = void;
		using result_type	 = Ret;
		using argument_types = std::tuple<Args...>;

		template <size_t I>
		using argument_type  = std::tuple_element_t<I, argument_types>;
	};

	template <typename TFunc>
	struct traits;
	
	// Functions (what's the use of this?)
	template <typename Ret, typename... Args>
	struct traits<Ret(Args...)>
		: traits_base <false, Ret, Args...>
	{
	};

	template <typename Ret, typename... Args>
	struct traits<Ret(Args...) noexcept>
		: traits_base<true, Ret, Args...>
	{
	};

	// Function Pointers
	template <typename Ret, typename... Args>
	struct traits<Ret(*)(Args...)> 
		:  traits_base<false, Ret, Args...>
	{
	};

	template <typename Ret, typename... Args>
	struct traits<Ret(*)(Args...) noexcept> 
		: traits_base<true, Ret, Args...>
	{
	};

	// Non-const member traitss
	template <typename Cls, typename Ret, typename... Args>
	struct traits<Ret(Cls::*)(Args...)> 
		: traits_base<false, Ret, Args...>
	{
		using class_type = Cls;
	};

	template <typename Cls, typename Ret, typename... Args>
	struct traits<Ret(Cls::*)(Args...) noexcept>
		: traits_base<true, Ret, Args...>
	{
		using class_type = Cls;
	};


	// Const member traitss
	template <typename Cls, typename Ret, typename... Args>
	struct traits<Ret(Cls::*)(Args...) const>
		: traits_base<false, Ret, Args...>
	{
		using class_type = Cls;
	};

	template <typename Cls, typename Ret, typename... Args>
	struct traits<Ret(Cls::*)(Args...) const noexcept>
		: traits_base<true, Ret, Args...>
	{
		using class_type = Cls;
	};

	template <typename TFunctor>
	struct traits : traits<decltype(&TFunctor::operator())>
	{
		using class_type = TFunctor;
	};

	template <typename TFunctor>
	struct traits<TFunctor&> : traits<TFunctor> {};

	template <typename TFunctor>
	struct traits<const TFunctor&> : traits<TFunctor> {};

	template <typename TFunctor>
	struct traits<TFunctor&&> : traits<TFunctor> {};
	
	template <typename TFunctor>
	struct traits<const TFunctor&&> : traits<TFunctor> {};
	
	template <typename TFunctor>
	struct traits<TFunctor*> : traits<TFunctor> {};

	template <typename TFunctor>
	struct traits<const TFunctor*> : traits<TFunctor> {};
}