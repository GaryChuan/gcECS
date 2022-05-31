#pragma once
#include <memory>
#include <ranges>
#include "../core/types.h"
#include "ecs_archetype.h"
#include "ecs_system.h"

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

		template <typename... TSystems>
		requires ( std::is_base_of_v<system::base, TSystems> && ...)
		constexpr void RegisterSystems() noexcept
		{
			(mSystemMgr.RegisterSystem<TSystems>(*this), ...);
		}

		template <typename TFunction>
		void for_each(TFunction&& callback) noexcept
		{
			using argument_types = 
				core::function::traits<TFunction>::argument_types;

			constexpr auto process_component = 
				[]<typename TComponent>() constexpr noexcept
				{
					return TComponent{};
				};

			[&]<typename... TComponents>(std::tuple<TComponents...>*) constexpr noexcept
			{
				callback
				(
					process_component.operator()<TComponents>()
					...
				);
			}(static_cast<argument_types*>(nullptr));

			/*
			an alternative to passing a pointer? 
			might need to overload one with a callback function
			
			or maybe make an equivalent to std::apply but one that doesnt accept in a tuple?

			constexpr auto process_components =
				types::process_tuple<argument_types>{};

			process_components(process_component);*/
		}

		// Could we maybe use ranges::views instead?
		std::ranges::ref_view<std::vector<archetype>> Search(std::span<const component::info* const> types)
		{
			return {};
		}

		void Run() noexcept
		{
			mSystemMgr.Run();
		}

	private:
		component::manager			   mComponentMgr{};
		system::manager				   mSystemMgr{};
		std::vector<archetype>		   mArchetypes{};
		std::unique_ptr<entity_info[]> mEntityInfos{};
	};
}