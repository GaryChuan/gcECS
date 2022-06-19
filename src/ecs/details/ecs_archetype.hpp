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
	template<typename TCallback>
	std::optional<component::entity> archetype::CreateEntity(TCallback&& callback) noexcept
	{
		using function_traits = core::function::traits<TCallback>;

		if (mECSMgr.CanCreateEntity() == false)
		{
			return std::nullopt;
		}

		return [&]<typename... TComponents>(std::tuple<TComponents...>*)
		{
			const int poolIndex = mPool.Append();
			const auto newEntity = mECSMgr.AllocNewEntity(poolIndex, *this);

			mPool.GetComponent<component::entity>(poolIndex) = newEntity;

			// Double checking if archetype has necessary bits
			assert(mComponentBits.GetBit(component::info_v<TComponents>.mUID) && ...);

			// Only run function if the callback accepts in parameters
			if constexpr (function_traits::argument_count > 0)
			{
				callback(mPool.GetComponent<std::remove_reference_t<TComponents>>(poolIndex)...);
			}

			return newEntity;
		}(static_cast<typename function_traits::argument_types*>(nullptr));
	}

	template <typename TComponent>
	[[nodiscard]] inline const TComponent& archetype::GetComponent(
		const std::uint32_t entityIndex) const noexcept
	{
		return mPool.GetComponent<TComponent>(entityIndex);
	}

	template <typename TFunction>
	void archetype::AccessGuard(TFunction&& function) noexcept
	{
		++mProcessReference;
		function();
		if (--mProcessReference == 0)
		{
			UpdateStructuralChanges();
		}
	}

	inline void archetype::UpdateStructuralChanges() noexcept
	{
		if (mToDelete.empty() == false)
		{
			for (const auto& entity : mToDelete)
			{
				const auto& entityDetails = mECSMgr.GetEntityDetails(entity);
				assert(entityDetails.mArchetype == this);

				mPool.Delete(entityDetails.mPoolIndex);

				if (entityDetails.mPoolIndex != mPool.size())
				{
					mECSMgr.SystemDeleteEntity(entity, mPool.GetComponent<component::entity>(entityDetails.mPoolIndex));
				}
				else
				{
					mECSMgr.SystemDeleteEntity(entity);
				}
			}
			mToDelete.clear();
		}
	}
}