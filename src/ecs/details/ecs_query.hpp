/******************************************************************************
filename: ecs_query.hpp
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs query.
******************************************************************************/
#pragma once

namespace ecs
{
	template <core::function::IsCallable TFunction>
	query::query(TFunction&& func) noexcept
	{
		Set<TFunction>(func);
	}

	template <typename... TComponents>
	void query::AddMustHaveComponents() noexcept
	{
		AddFromComponents<TComponents...>(mMustHave);
	}

	template <typename... TComponents>
	void query::AddHaveOneOfComponents() noexcept
	{
		AddFromComponents<TComponents...>(mHaveOneOf);
	}

	template <typename... TComponents>
	void query::AddHaveNoneOfComponents() noexcept
	{
		AddFromComponents<TComponents...>(mHaveNoneOf);
	}

	bool query::Compare(const ArchetypeSignature& archetypeSignature) const noexcept
	{
		std::uint64_t c = 0;
		std::uint64_t a = 0;

		for (int i = 0, end = static_cast<int>(archetypeSignature.size()); i < end; ++i)
		{
			if (mHaveNoneOf[i] & archetypeSignature[i])
			{
				return false;
			}

			if ((mMustHave[i] & archetypeSignature[i]) != mMustHave[i])
			{
				return false;
			}

			c |= (mHaveOneOf[i] & archetypeSignature[i]);
			a |= mHaveOneOf[i];
		}

		assert(!(!a && c));
		;
		return !a || c;
	}

	template <core::function::IsCallable TFunction>
	void query::Set(TFunction&& func) noexcept
	{
		using argument_types =
			core::function::traits<TFunction>::argument_types;

		[this] <typename... TComponents>(std::tuple<TComponents...>*)
		{
			([this]<component::is_valid_type TComponent>(std::tuple<TComponent>*)
			{
				if constexpr (std::is_pointer_v<TComponent>)
				{
					mHaveOneOf.SetBit(component::info_v<TComponent>.mUID);
				}
				else if constexpr (std::is_reference_v<TComponent>)
				{
					mMustHave.SetBit(component::info_v<TComponent>.mUID);
				}
			}(static_cast<std::tuple<TComponents>*>(nullptr)), ...);
		}(static_cast<argument_types*>(nullptr));
	}

	template <typename... TQueries>
	void query::Set(std::tuple<TQueries...>*) noexcept
	{
		([&]<template <typename...> class TQuery, typename... TComponents>(TQuery<TComponents...>*)
		{
			using TQueryType = TQuery<TComponents...>;

			if constexpr (std::is_same_v<TQueryType, query::must_have<TComponents...>>)
			{
				AddFromComponents<TComponents...>(mMustHave);
			}
			else if constexpr (std::is_same_v<TQueryType, query::have_one_of<TComponents...>>)
			{
				AddFromComponents<TComponents...>(mHaveOneOf);
			}
			else if constexpr (std::is_same_v<TQueryType, query::have_none_of<TComponents...>>)
			{
				AddFromComponents<TComponents...>(mHaveNoneOf);
			}
			else
			{
				static_assert(std::false_type, 
							  "None of the queries are must_have, have_none_of, have_one_of types!");
			}
		}(static_cast<TQueries*>(nullptr)), ...);
	}

	template <typename... TComponents>
	void query::AddFromComponents(ArchetypeSignature& signature) noexcept
	{
		(signature.SetBit(component::info_v<TComponents>.mUID), ...);
	}

	template<typename ...TQueries>
	query make_query() noexcept
	{
		query query{};

		query.Set(static_cast<std::tuple<TQueries...>*>(nullptr));

		return query;
	}
}