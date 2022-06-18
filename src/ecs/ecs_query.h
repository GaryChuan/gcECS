#pragma once

#include "../core/bitarray.h"
#include "../core/functions.h"
#include "ecs_component.h"
#include "ecs_settings.h"

#include <iostream>

namespace ecs
{
	struct query final
	{
		template <typename... TComponents>
		struct must_have final {};

		template <typename... TComponents>
		struct have_one_of final {};

		template <typename... TComponents>
		struct have_none_of final {};

		using ArchetypeSignature = core::bitarray<settings::max_component_types>;

		query() = default;

		template <typename TFunction>
		query(TFunction&& func) noexcept
		{
			Set<TFunction>(func);
		}

		template <typename... TComponents>
		void AddMustHaveComponents() noexcept
		{
			AddFromComponents(mMustHave);
		}

		template <typename... TComponents>
		void AddHaveOneOfComponents() noexcept
		{
			AddFromComponents(mHaveOneOf);
		}

		template <typename... TComponents>
		void AddHaveNoneOfComponents() noexcept
		{
			AddFromComponents(mHaveNoneOf);
		}


		bool Compare(const ArchetypeSignature& archetypeSignature) const noexcept
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
		
		template <typename TFunction>
		void Set(TFunction&& func) noexcept
		{
			using argument_types =
				core::function::traits<TFunction>::argument_types;

			std::cout << typeid(argument_types).name() << std::endl;

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
		void Set(std::tuple<TQueries...>*) noexcept
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
			}(static_cast<TQueries*>(nullptr)), ...);
		}

	private:
		template <typename... TComponents>
		void AddFromComponents(ArchetypeSignature& signature) noexcept
		{
			(signature.SetBit(component::info_v<TComponents>.mUID), ...);
		}

	private:
		ArchetypeSignature mMustHave{};
		ArchetypeSignature mHaveOneOf{};
		ArchetypeSignature mHaveNoneOf{};
	};
}