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
			int mPoolIndex{-1};
			int	mNextFreeEntity;
		};

		component::entity::validation mValidation{};
	};

	class manager final
	{
	public:
		inline manager() noexcept;

		inline manager(const manager&) noexcept = delete;

		template <ecs::component::is_valid_type... TComponents>
		requires ((sizeof(TComponents) <= 4096) && ...)
		inline constexpr void RegisterComponents() noexcept;

		template <typename... TSystems>
		requires (std::is_base_of_v<system::base, TSystems> && ...)
		inline constexpr void RegisterSystems() noexcept;

		template <core::function::IsCallable TFunction>
		requires (std::is_same_v<typename core::function::traits<TFunction>::return_type, void>)
		inline void ForEach(const std::vector<archetype*>& list, TFunction&& callback) const noexcept;

		template <core::function::IsCallable TFunction>
		requires (std::is_same_v<typename core::function::traits<TFunction>::return_type, bool>)
		inline void ForEach(const std::vector<archetype*>& list, TFunction&& callback) const noexcept;

		template <typename... TComponents>
		[[nodiscard]] inline archetype& GetArchetype() noexcept;

		[[nodiscard]] inline  archetype& GetArchetype(
			std::span<const component::info* const> component_list) noexcept;

		[[nodiscard]] inline std::vector<archetype*> Search(const query& query);

		[[nodiscard]] inline bool CanCreateEntity() const noexcept;

		inline void Run() noexcept;

		[[nodiscard]] inline const entity_info& GetEntityDetails(
			const component::entity& entity) const noexcept;

		inline void DeleteEntity(component::entity& entityToDelete) noexcept;

	private:
		friend archetype;

		inline void SystemDeleteEntity(
			const component::entity& deletedEntity, 
			const component::entity& swappedEntity) noexcept;

		inline void SystemDeleteEntity(const component::entity& deletedEntity) noexcept;

		template <typename... TComponents>
		[[nodiscard]] inline const auto& GetComponentList() const noexcept;

		[[nodiscard]] inline component::entity AllocNewEntity(int poolIndex, archetype& archetype) noexcept;

	private:
		int											mNextFreeEntity{};
		component::manager							mComponentMgr{};
		system::manager								mSystemMgr{};
		std::vector<std::unique_ptr<archetype>>		mArchetypes{};
		std::unique_ptr<entity_info[]>				mEntityInfos{};
	};
}