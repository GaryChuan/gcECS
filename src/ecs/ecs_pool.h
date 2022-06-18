/******************************************************************************
filename: ecs_pool.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs pool which manages allocation
and deallocation of virtual memory.
******************************************************************************/
#pragma once
#include <span>
#include <array>
#include "ecs_component.h"
#include "ecs_settings.h"

namespace ecs
{
	class manager;

	class pool
	{
	public:
		pool() noexcept = default;
		pool(const pool&) noexcept = delete;
		~pool() noexcept;

		pool(std::span<const component::info* const> infos) noexcept;

		void Clear() noexcept;
		int Append() noexcept;
		void Delete(std::uint32_t index) noexcept;
		
		std::uint32_t size() const noexcept;

		template <typename TComponent>
		requires (std::is_same_v<TComponent, std::decay_t<TComponent>>)
		__inline TComponent& GetComponent(const std::uint32_t entityIndex) noexcept;

		template <typename TComponent>
		requires (std::is_same_v<TComponent, std::decay_t<TComponent>>)
		__inline const TComponent& GetComponent(const std::uint32_t entityIndex) const noexcept;

	private:
		friend manager;

		const unsigned GetPageFromIndex(
			const component::info& info, 
			int entityIndex) const noexcept;

		const int FindComponentIndexFromUID(const std::uint16_t UID) const noexcept;

	private:
		std::span<const component::info* const> mInfos{};
		std::array<std::byte*, settings::max_component_per_entities> mComponents{};
		std::uint32_t mSize{}; // entity count
	};
}

#include "details/ecs_pool.hpp"