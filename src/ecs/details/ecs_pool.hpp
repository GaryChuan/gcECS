/******************************************************************************
filename: ecs_pool.hpp
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs pool functions for getting
components.
******************************************************************************/
#pragma once

namespace ecs
{
	template <typename TComponent>
	requires (std::is_same_v<TComponent, std::decay_t<TComponent>>)
	TComponent& pool::GetComponent(const std::uint32_t entityIndex) noexcept
	{
		const auto componentIndex = FindComponentIndexFromUID(component::info_v<TComponent>.mUID);
		return *reinterpret_cast<TComponent*>(&mComponents[componentIndex][entityIndex * sizeof(TComponent)]);
	}

	template <typename TComponent>
	requires (std::is_same_v<TComponent, std::decay_t<TComponent>>)
	inline const TComponent& pool::GetComponent(const std::uint32_t entityIndex) const noexcept
	{
		const auto componentIndex = FindComponentIndexFromUID(component::info_v<TComponent>.mUID);
		return *reinterpret_cast<TComponent*>(&mComponents[componentIndex][entityIndex * sizeof(TComponent)]);
	}
}