/******************************************************************************
filename: functions.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of types that allow for decomposing
functions into its relative traits - such as its signature, class type, 
return type, and argument types.
******************************************************************************/
#pragma once
#include "types.h"

namespace core::function
{
	namespace details
	{
		template <typename... T>
		constexpr bool always_false = false;
	}

	template <typename T>
	concept HasFunctionOperator = requires { T::operator(); };

	namespace details
	{
		template <typename T>
		struct is_callable
		{
			static constexpr auto value = std::conditional_t<HasFunctionOperator<T>, std::true_type, std::false_type>::value;
		};

		template <typename T>
		constexpr auto& is_callable_v = std::conditional_t<std::is_class<T>, is_callable<T>, std::is_function<T>>::value;
	}

	template <typename T>
	constexpr auto& is_callable_v = details::is_callable_v<core::types::full_decay_t<T>>;

	template <bool NoExcept, typename Ret, typename... Args>
	struct traits_base
	{
		using signature		 = Ret(Args...) noexcept(NoExcept);
		using class_type	 = void;
		using return_type	 = Ret;
		using argument_types = std::tuple<Args...>;

		static constexpr auto argument_count = sizeof...(Args);

		template <size_t I>
		using argument_type  = std::tuple_element_t<I, argument_types>;
	};

	template <typename TFunc>
	struct traits;
	
	// Functions
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

	// Non-const member functions
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
	struct traits<const TFunctor&&> : traits<TFunctor> { static_assert(details::always_false<TFunctor>, "const TFunctor&& is not allowed!"); };

	template <typename TFunctor>
	struct traits<TFunctor*> : traits<TFunctor> {};

	template <typename TFunctor>
	struct traits<const TFunctor*> : traits<TFunctor> {};
}