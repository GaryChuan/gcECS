/******************************************************************************
filename: ecs_archetype.cpp
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs archetype.
******************************************************************************/
#include "ecs_archetype.h"
#include "ecs_manager.h"

namespace ecs
{
	archetype::archetype(ecs::manager& ecsMgr,
						 std::span<const component::info* const> component_list,
						 const bits& component_bits) noexcept
		: mComponentBits { component_bits }
		, mPool { component_list }
		, mECSMgr { ecsMgr }
	{
	}

	void archetype::DestroyEntity(component::entity& entityToDestroy) noexcept
	{
		assert(entityToDestroy.isZombie() == false);

		entityToDestroy.mValidation.mZombie = true;
		
		mToDelete.push_back(entityToDestroy);

		if (mProcessReference == 0)
		{
			UpdateStructuralChanges();
		}
	}

	const typename archetype::bits& archetype::GetBits() const noexcept
	{
		return mComponentBits;
	}

	bool archetype::CompareBits(const bits& bits) const noexcept
	{
		return mComponentBits.Compare(bits);
	}
}