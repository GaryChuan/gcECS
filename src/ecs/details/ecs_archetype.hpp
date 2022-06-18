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