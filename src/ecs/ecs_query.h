/******************************************************************************
filename: ecs_query.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of query as well as declarations of
its subclasses must_have, have_one_of, and have_none_of, which are necessary
for filtering queries.
******************************************************************************/
#pragma once

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
		__inline query(TFunction&& func) noexcept;

		template <typename... TComponents>
		__inline void AddMustHaveComponents() noexcept;

		template <typename... TComponents>
		__inline void AddHaveOneOfComponents() noexcept;

		template <typename... TComponents>
		__inline void AddHaveNoneOfComponents() noexcept;

		__inline bool Compare(const ArchetypeSignature& archetypeSignature) const noexcept;
		
		template <typename TFunction>
		__inline void Set(TFunction&& func) noexcept;

		template <typename... TQueries>
		__inline void Set(std::tuple<TQueries...>*) noexcept;

	private:
		template <typename... TComponents>
		__inline void AddFromComponents(ArchetypeSignature& signature) noexcept;

	private:
		ArchetypeSignature mMustHave{};
		ArchetypeSignature mHaveOneOf{};
		ArchetypeSignature mHaveNoneOf{};
	};
}

#include "details/ecs_query.hpp"