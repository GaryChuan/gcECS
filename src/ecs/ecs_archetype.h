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

		void Initialize(std::span<const component::info* const> component_list);
		void Initialize(std::span<const component::info* const> component_list,
						const bits& component_bits);

		template <typename TComponent>
		const TComponent& GetComponent(const std::uint32_t entityIndex) const noexcept
		{
			return mPool.GetComponent<TComponent>(entityIndex);
		}

		template <typename TFunction>
		inline void AccessGuard(TFunction&& function, ecs::manager& ecsMgr) noexcept
		{
			++mProcessReference;
			function();
			if (--mProcessReference == 0)
			{
				UpdateStructuralChanges(ecsMgr);
			}
		}

		void DestroyEntity(component::entity& entityToDestroy, ecs::manager& ecsMgr) noexcept;

		void UpdateStructuralChanges(ecs::manager& ecsMgr) noexcept;


		int		mProcessReference = 0;

		bits	mComponentBits{};
		pool	mPool{};

		std::vector<component::entity> mToDelete{};
	};
}