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
	
	// Functions (what's the use of this?)
	template <typename Ret, typename... Args>
	struct function<Ret(Args...)>
		: function_base <Ret(Args...)>
	{
		using type = Ret(Args...);
	};

	template <typename Ret, typename... Args>
	struct function<Ret(Args...) noexcept>
		: function_base<Ret(Args...) noexcept>
	{
		using type = Ret(Args...) noexcept;
	};

	// Funct3ion Pointers
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

	// Non-const member functions
	template <typename Cls, typename Ret, typename... Args>
	struct function<Ret(Cls::*)(Args...)> 
		: function_base<Ret(Cls::*)(Args...)>
	{
		using type = Ret(Cls::*)(Args...);
	};

	template <typename Cls, typename Ret, typename... Args>
	struct function<Ret(Cls::*)(Args...) noexcept>
		: function_base<Ret(Cls::*)(Args...) noexcept>
	{
		using type = Ret(Cls::*)(Args...) noexcept;
	};


	// Const member functions
	template <typename Cls, typename Ret, typename... Args>
	struct function<Ret(Cls::*)(Args...) const>
		: function_base<Ret(Cls::*)(Args...) const>
	{
		using type = Ret(Cls::*)(Args...) const;
	};

	template <typename Cls, typename Ret, typename... Args>
	struct function<Ret(Cls::*)(Args...) const noexcept>
		: function_base<Ret(Cls::*)(Args...) const noexcept>
	{
		using type = Ret(Cls::*)(Args...) const noexcept;
	};

	template <typename TFunctor>
	struct function : function_base<decltype(&TFunctor::operator())>
	{
		using type = decltype(&TFunctor::operator());
	};

	template <typename TFunctor>
	struct function<TFunctor&> : function<TFunctor> {};

	template <typename TFunctor>
	struct function<const TFunctor&> : function<TFunctor> {};

	template <typename TFunctor>
	struct function<TFunctor&&> : function<TFunctor> {};
	
	template <typename TFunctor>
	struct function<const TFunctor&&> : function<TFunctor> {};
	
	template <typename TFunctor>
	struct function<TFunctor*> : function<TFunctor> {};

	template <typename TFunctor>
	struct function<const TFunctor*> : function<TFunctor> {};
}