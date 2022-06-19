/******************************************************************************
filename: ecs_archetype.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the declaration of ecs archetype.
******************************************************************************/
#pragma once
#include <span>
#include <optional>
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

		archetype(const archetype&) noexcept = delete;

		archetype(ecs::manager& ecsMgr,
				  std::span<const component::info* const> component_list,
				  const bits& component_bits) noexcept;

		[[nodiscard]] const bits& GetBits() const noexcept;
		[[nodiscard]] bool CompareBits(const bits& bits) const noexcept;

		template <typename TCallback>
		inline std::optional<component::entity> CreateEntity(TCallback&& callback) noexcept;

	private:
		friend manager;

		template <typename TComponent>
		[[nodiscard]] inline const TComponent& GetComponent(
			const std::uint32_t entityIndex) const noexcept;

		template <typename TFunction>
		inline void AccessGuard(TFunction&& function) noexcept;

		void DestroyEntity(component::entity& entityToDestroy) noexcept;

		inline void UpdateStructuralChanges() noexcept;
	
	private:
		int		mProcessReference = 0;

		bits	mComponentBits{};
		pool	mPool{};

		ecs::manager& mECSMgr;
		std::vector<component::entity> mToDelete{};
	};
}