#pragma once
#include <memory>
#include <ranges>
#include <array>
#include "../core/types.h"
#include "ecs_query.h"
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

		template <ecs::component::is_valid_type... TComponents>
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
		std::vector<archetype*> Search(TFunction&& func) const noexcept
		{
			using argument_types =
				core::function::traits<TFunction>::argument_types;

			Query query{};
			
			// query.Set(std::forward<TFunction>(func));


			return {};
		}

		template <typename TFunction>
		void for_each(const std::vector<archetype*>& list, TFunction&& callback) const noexcept
		{
			using argument_types = core::function::traits<TFunction>::argument_types;

			for (const auto& archetype : list)
			{
				const auto& pool = archetype->mPool;

				auto cache_pointers = [&]<typename... TComponents>(std::tuple<TComponents...>*) constexpr noexcept 
				{
					return std::array<std::byte*, sizeof...(TComponents)>
					{
						[&]<typename TComponent>(std::tuple<TComponent>*) constexpr noexcept
						{
							const auto I = pool.FindComponentIndexFromUID(component::info_v<TComponent>.mUID);
							
							if constexpr (std::is_pointer_v<TComponent>)
							{
								return (I < 0) ? static_cast<std::byte*>(nullptr) : pool.mComponents[I];
							}
							else
							{
								return pool.mComponents[I];
							}
						}(static_cast<std::tuple<TComponents>*>(nullptr))
						...
					};
				}(static_cast<argument_types*>(nullptr));

				[&] <typename... TComponents>(std::tuple<TComponents...>*) constexpr noexcept
				{
					callback
					(
						[&]<typename TComponent>(std::tuple<TComponent>*) constexpr noexcept -> TComponent
						{
							constexpr auto I = core::types::tuple_type_to_index_v<TComponent, argument_types>;
							auto& pComponent = cache_pointers[I];

							if constexpr (std::is_pointer_v<TComponent>)
							{
								if (pComponent == nullptr) return reinterpret_cast<TComponent>(nullptr);
							}

							auto p = pComponent;
							pComponent += sizeof(std::decay_t<TComponent>);

							return std::is_pointer_v<TComponent> ? reinterpret_cast<TComponent>(p) : reinterpret_cast<TComponent>(*p);
						}(static_cast<std::tuple<TComponents>*>(nullptr))
						...
					);
				}(static_cast<argument_types*>(nullptr));
			}
			// Get cache pointers

			// Process components
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