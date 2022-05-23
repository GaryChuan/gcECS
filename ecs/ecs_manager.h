#pragma once
#include <memory>
#include "ecs_archetype.h"

namespace ecs
{
	struct entity_info final
	{
		archetype*						mArchetype{};
		std::uint32_t					mPoolIndex{};
		component::entity::validation	mValidation{};
	};

	class manager final
	{
	public:
		manager() noexcept
			: mEntityInfos{ std::make_unique<entity_info[]>(settings::max_entities) }
		{
			mComponentMgr.RegisterComponent<component::entity>();
		}

		manager(const manager&) = delete;

		template <typename... TComponents>
		requires ((sizeof(TComponents) <= 4096) && ...)
			constexpr void RegisterComponents() noexcept
		{
			(mComponentMgr.RegisterComponent<TComponents>(), ...);
		}

	private:
		component::manager			   mComponentMgr{};
		std::unique_ptr<entity_info[]> mEntityInfos{};
	};
}