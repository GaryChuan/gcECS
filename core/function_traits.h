#pragma once
#include <tuple>
#include <concepts>

namespace core::function
{
	template <typename Ret, typename... Args>
	struct traits_base
	{
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
		: traits_base <Ret(Args...)>
	{
		using type = Ret(Args...);
	};

	template <typename Ret, typename... Args>
	struct traits<Ret(Args...) noexcept>
		: traits_base<Ret(Args...) noexcept>
	{
		using type = Ret(Args...) noexcept;
	};

	// Function Pointers
	template <typename Ret, typename... Args>
	struct traits<Ret(*)(Args...)> 
		:  traits_base<Ret(*)(Args...)>
	{
		using type = Ret(*)(Args...);
	};

	template <typename Ret, typename... Args>
	struct traits<Ret(*)(Args...) noexcept> 
		: traits_base<Ret(*)(Args...) noexcept>
	{
		using type = Ret(*)(Args...) noexcept;
	};

	// Non-const member traitss
	template <typename Cls, typename Ret, typename... Args>
	struct traits<Ret(Cls::*)(Args...)> 
		: traits_base<Ret(Cls::*)(Args...)>
	{
		using type = Ret(Cls::*)(Args...);
	};

	template <typename Cls, typename Ret, typename... Args>
	struct traits<Ret(Cls::*)(Args...) noexcept>
		: traits_base<Ret(Cls::*)(Args...) noexcept>
	{
		using type = Ret(Cls::*)(Args...) noexcept;
	};


	// Const member traitss
	template <typename Cls, typename Ret, typename... Args>
	struct traits<Ret(Cls::*)(Args...) const>
		: traits_base<Ret(Cls::*)(Args...) const>
	{
		using type = Ret(Cls::*)(Args...) const;
	};

	template <typename Cls, typename Ret, typename... Args>
	struct traits<Ret(Cls::*)(Args...) const noexcept>
		: traits_base<Ret(Cls::*)(Args...) const noexcept>
	{
		using type = Ret(Cls::*)(Args...) const noexcept;
	};

	template <typename TFunctor>
	struct traits : traits_base<decltype(&TFunctor::operator())>
	{
		using type = decltype(&TFunctor::operator());
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