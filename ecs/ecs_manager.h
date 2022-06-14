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
		}

		/*template <typename TCallback, typename... TComponents>
		requires ( (std::is_same_v<TComponents, component::entity> == false) && ... )
		component::entity CreateEntity(TCallback&& callback)
		{
			

			return CreateEntity(callback, component_list);
		}*/

		template <typename... TComponents>
		requires ((std::is_same_v<TComponents, component::entity> == false) && ...)
		inline archetype& GetArchetype() noexcept
		{
			static constexpr auto component_list = std::array
			{
				&component::info_v<component::entity>,
				&component::info_v<TComponents> ...
			};

			return GetArchetype(component_list);
		}

		inline archetype& GetArchetype(std::span<const component::info* const> component_list) noexcept
		{
			// Create or get archetype
			core::bitarray<settings::max_component_types> component_bits{};

			for (const auto& component : component_list)
			{
				assert(component->mUID != component::info::invalid_id);
				component_bits.SetBit(component->mUID);
			}

			// Ensure entity is part of components bits
			assert(component_bits.GetBit(component::info_v<component::entity>.mUID));

			auto archetype_itr = std::ranges::find_if(mArchetypes,
				[&component_bits](const auto& archetype)
				{
					return archetype->CompareBits(component_bits);
				});

			// Unable to find archetype, create a new one
			if (archetype_itr == mArchetypes.end())
			{
				mArchetypes.emplace_back(std::make_unique<archetype>());
				archetype_itr = std::prev(mArchetypes.end());
				(*archetype_itr)->Initialize(component_list, component_bits);
			}

			return **archetype_itr;
		}

		// Could we maybe use ranges::views instead?
		std::ranges::ref_view<std::vector<archetype>> Search(std::span<const component::info* const> component_list)
		{
			return {};
		}

		void Run() noexcept
		{
			mSystemMgr.Run();
		}

		template <typename TCallback>
		inline component::entity CreateEntity(TCallback&& callback, archetype& archetype) noexcept
		{
			using argument_types = core::function::traits<TCallback>::argument_types;

			return[&]<typename... TComponents>(std::tuple<TComponents...>*)
			{
				const int poolIndex = archetype.mPool.Append();
				const auto newEntity = AllocNewEntity(poolIndex, archetype);

				archetype.mPool.GetComponent<component::entity>(poolIndex) = newEntity;

				// Double checking if archetype has necessary bits
				assert(archetype.mComponentBits.GetBit(component::info_v<TComponents>.mUID) && ...);
				
				// Only run function if the callback accepts in parameters
				if constexpr (std::tuple_size_v<argument_types> != 0)
				{
					callback(archetype.mPool.GetComponent<std::remove_reference_t<TComponents>>(poolIndex)...);
				}

				return newEntity;
			}(static_cast<argument_types*>(nullptr));
		}

	private:
		inline component::entity AllocNewEntity(int poolIndex, archetype& archetype)
		{
			assert(mFreeEntities >= 0);

			const int entityIndex = mFreeEntities;
			auto& newEntity = mEntityInfos[entityIndex];

			mFreeEntities = newEntity.mPoolIndex;

			newEntity.mPoolIndex = poolIndex;
			newEntity.mArchetype = &archetype;

			return component::entity
			{
				.mGlobalID = static_cast<uint32_t>(entityIndex),
				.mValidation = newEntity.mValidation
			};
		}
	private:
		int											mFreeEntities;
		component::manager							mComponentMgr{};
		system::manager								mSystemMgr{};
		std::vector<std::unique_ptr<archetype>>		mArchetypes{};
		std::unique_ptr<entity_info[]>				mEntityInfos{};
	};
}