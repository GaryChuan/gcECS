/******************************************************************************
filename: ecs_manager.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of the ecs manager. It includes the
implementation of the ecs manager and details of entity_info which contains
global entity information.
******************************************************************************/
#pragma once
#include <memory>
#include <array>
#include "types.h"
#include "ecs_query.h"
#include "ecs_archetype.h"
#include "ecs_system.h"

namespace ecs
{
	struct entity_info final
	{
		archetype* mArchetype{};

		union
		{
			std::uint32_t	mPoolIndex{};
			std::uint32_t	mNextFreeEntity;
		};

		component::entity::validation mValidation{};
	};

	class manager final
	{
	public:
		__inline manager() noexcept;

		__inline manager(const manager&) noexcept = delete;

		template <ecs::component::is_valid_type... TComponents>
		requires ((sizeof(TComponents) <= 4096) && ...)
		__inline constexpr void RegisterComponents() noexcept;

		template <typename... TSystems>
		requires (std::is_base_of_v<system::base, TSystems> && ...)
		__inline constexpr void RegisterSystems() noexcept;

		template <typename TFunction>
		requires (std::is_same_v<typename core::function::traits<TFunction>::return_type, void>)
		__inline void for_each(const std::vector<archetype*>& list, TFunction&& callback) noexcept;

		template <typename TFunction>
		requires (std::is_same_v<typename core::function::traits<TFunction>::return_type, bool>)
		__inline void for_each(const std::vector<archetype*>& list, TFunction&& callback) noexcept;

		template <typename... TComponents>
		requires ((std::is_same_v<TComponents, component::entity> == false) && ...)
		[[nodiscard]] __inline  archetype& GetArchetype() noexcept;

		[[nodiscard]] __inline  archetype& GetArchetype(std::span<const component::info* const> component_list) noexcept;

		[[nodiscard]] __inline std::vector<archetype*> Search(const query& query);

		__inline void Run() noexcept;

		template <typename TCallback>
		__inline component::entity CreateEntity(TCallback&& callback, archetype& archetype) noexcept;

		[[nodiscard]] __inline const entity_info& GetEntityDetails(const component::entity& entity) const noexcept;

		__inline void DeleteEntity(component::entity& entityToDelete) noexcept;

		__inline void SystemDeleteEntity(const component::entity& deletedEntity, const component::entity& swappedEntity) noexcept;

		__inline void SystemDeleteEntity(const component::entity& deletedEntity);

	private:
		template <typename... TComponents>
		static constexpr auto component_list = std::array{ &component::info_v<TComponents>... };

		[[nodiscard]] __inline component::entity AllocNewEntity(int poolIndex, archetype& archetype) noexcept;

	private:
		int											mNextFreeEntity{};
		component::manager							mComponentMgr{};
		system::manager								mSystemMgr{};
		std::vector<std::unique_ptr<archetype>>		mArchetypes{};
		std::unique_ptr<entity_info[]>				mEntityInfos{};
	};
}

#include "details/ecs_manager.hpp"