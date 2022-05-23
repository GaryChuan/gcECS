#pragma once
#include <tuple>

namespace core::traits
{
	template <typename Ret, typename... Args>
	struct function_base
	{
		using result_type	 = Ret;
		using argument_types = std::tuple<Args...>;

		template <size_t I>
		using argument_type  = std::tuple_element_t<I, argument_types>;
	};

	template <typename TFunc>
	struct function;

	template <typename Ret, typename... Args>
	struct function<Ret(*)(Args...)> 
		:  function_base<Ret(*)(Args...)>
	{
		using type = Ret(*)(Args...);
	};

	template <typename Ret, typename... Args>
	struct function<Ret(*)(Args...) noexcept> 
		: function_base<Ret(*)(Args...) noexcept>
	{
		using type = Ret(*)(Args...) noexcept;
	};

	template <typename Cls, typename Ret, typename... Args>
	struct function<Ret(Cls::*)(Args...)> 
		: function_base<Ret(Cls::*)(Args...)>
	{
		using type = Ret(Cls::*)(Args...);
	};

	template <typename Cls, typename Ret, typename... Args>
	struct function<Ret(Cls::*)(Args...) const>
		: function_base<Ret(Cls::*)(Args...) const>
	{
		using type = Ret(Cls::*)(Args...) const;
	};

	template <typename Cls, typename Ret, typename... Args>
	struct function<Ret(Cls::*)(Args...) noexcept>
		: function_base<Ret(Cls::*)(Args...) noexcept>
	{
		using type = Ret(Cls::*)(Args...) noexcept;
	};

	template <typename Cls, typename Ret, typename... Args>
	struct function<Ret(Cls::*)(Args...) const noexcept>
		: function_base<Ret(Cls::*)(Args...) const noexcept>
	{
		using type = Ret(Cls::*)(Args...) const noexcept;
	};
}