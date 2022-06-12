#pragma once
#include <span>
#include "ecs_pool.h"

namespace ecs
{
	class manager;

	class archetype
	{
	public:
		archetype() = default;
		archetype(const archetype&) = delete;

		void Initialize(std::span<const component::info* const> infos);

		void CreateEntity();
		void DeleteEntity();

	private:
		friend manager;

		pool mPool;
	};
}