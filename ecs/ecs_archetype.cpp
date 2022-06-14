#include "ecs_archetype.h"
#include "ecs_manager.h"

namespace ecs
{
	/*archetype::archetype(manager& ecsMgr) noexcept
		: mECSMgr { ecsMgr }
	{
	}*/

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

	/*void archetype::CreateEntity()
	{
		mPool.Append();
	}

	void archetype::DeleteEntity()
	{

	}*/
	const typename archetype::bits& archetype::GetBits() const noexcept
	{
		return mComponentBits;
	}

	bool archetype::CompareBits(const bits& bits) const noexcept
	{
		return mComponentBits.Compare(bits);
	}
}