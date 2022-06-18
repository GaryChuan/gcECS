/******************************************************************************
filename: ecs_archetype.hpp
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs archetype.
******************************************************************************/
#pragma once

namespace ecs
{
	template <typename TComponent>
	[[nodiscard]] __inline const TComponent& archetype::GetComponent(
		const std::uint32_t entityIndex) const noexcept
	{
		return mPool.GetComponent<TComponent>(entityIndex);
	}

	template <typename TFunction>
	__inline void archetype::AccessGuard(
		TFunction&& function, ecs::manager& ecsMgr) noexcept
	{
		++mProcessReference;
		function();
		if (--mProcessReference == 0)
		{
			UpdateStructuralChanges(ecsMgr);
		}
	}
}