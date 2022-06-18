/******************************************************************************
filename: ecs_archetype.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs archetype.
******************************************************************************/
#pragma once
#include <span>
#include <vector>
#include "functions.h"
#include "bitarray.h"
#include "ecs_pool.h"

namespace ecs
{
	class manager;

	class archetype
	{
	public:
		using bits = core::bitarray<settings::max_component_types>;

		// archetype(manager& ecsMgr) noexcept;
		archetype() noexcept = default;
		archetype(const archetype&) noexcept = delete;

		[[nodiscard]] const bits& GetBits() const noexcept;
		[[nodiscard]] bool CompareBits(const bits& bits) const noexcept;

	private:
		friend manager;

		template <typename TComponent>
		[[nodiscard]] __inline const TComponent& GetComponent(
			const std::uint32_t entityIndex) const noexcept;

		template <typename TFunction>
		__inline void AccessGuard(
			TFunction&& function, ecs::manager& ecsMgr) noexcept;

		void Initialize(
			std::span<const component::info* const> component_list) noexcept;
		
		void Initialize(
			std::span<const component::info* const> component_list,
			const bits& component_bits) noexcept;

		void DestroyEntity(
			component::entity& entityToDestroy, ecs::manager& ecsMgr) noexcept;

		void UpdateStructuralChanges(ecs::manager& ecsMgr) noexcept;
	
	private:
		int		mProcessReference = 0;

		bits	mComponentBits{};
		pool	mPool{};

		std::vector<component::entity> mToDelete{};
	};
}

#include "details/ecs_archetype.hpp""