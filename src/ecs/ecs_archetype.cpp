/******************************************************************************
filename: ecs_archetype.cpp
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs archetype functions.
******************************************************************************/
#include "ecs_archetype.h"
#include "ecs_manager.h"

namespace ecs
{
	void archetype::Initialize(std::span<const component::info* const> component_list)
	{
		mPool.Initialize(component_list);
	}

	void archetype::Initialize(std::span<const component::info* const> component_list, 
							   const bits& component_bits)
	{
		Initialize(component_list);
		mComponentBits = component_bits;
	}

	void archetype::DestroyEntity(component::entity& entityToDestroy, ecs::manager& ecsMgr) noexcept
	{
		assert(entityToDestroy.isZombie() == false);

		entityToDestroy.mValidation.mZombie = true;
		
		mToDelete.push_back(entityToDestroy);

		if (mProcessReference == 0)
		{
			UpdateStructuralChanges(ecsMgr);
		}
	}

	void archetype::UpdateStructuralChanges(ecs::manager& ecsMgr) noexcept
	{
		if (mToDelete.empty() == false)
		{
			for (const auto& entity : mToDelete)
			{
				const auto& entityDetails = ecsMgr.GetEntityDetails(entity);
				assert(entityDetails.mArchetype == this);

				mPool.Delete(entityDetails.mPoolIndex);

				if (entityDetails.mPoolIndex != mPool.size())
				{
					ecsMgr.SystemDeleteEntity(entity, mPool.GetComponent<component::entity>(entityDetails.mPoolIndex));
				}
				else
				{
					ecsMgr.SystemDeleteEntity(entity);
				}
			}
			mToDelete.clear();
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