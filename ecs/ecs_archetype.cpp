#include "ecs_archetype.h"

namespace ecs
{
	void archetype::Initialize(std::span<const component::info* const> infos)
	{
		mPool.Initialize(infos);
	}

	void archetype::CreateEntity()
	{
		mPool.Append();
	}

	void archetype::DeleteEntity()
	{

	}
}