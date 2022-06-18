/******************************************************************************
filename: ecs_manager.hpp
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs manager.
******************************************************************************/
#pragma once

namespace ecs
{
	manager::manager() noexcept
		: mEntityInfos{ std::make_unique<entity_info[]>(settings::max_entities) }
	{
		mComponentMgr.RegisterComponent<component::entity>();

		// Create link list of empty entries
		for (int i = 0, end = settings::max_entities - 2; i < end; ++i)
		{
			mEntityInfos[i].mNextFreeEntity = i + 1;
		}
	}

	template <ecs::component::is_valid_type... TComponents>
	requires ((sizeof(TComponents) <= 4096) && ...)
	constexpr void manager::RegisterComponents() noexcept
	{
		(mComponentMgr.RegisterComponent<TComponents>(), ...);
	}

	template <typename... TSystems>
	requires (std::is_base_of_v<system::base, TSystems> && ...)
		inline constexpr void manager::RegisterSystems() noexcept
	{
		(mSystemMgr.RegisterSystem<TSystems>(*this), ...);
	}

	template <core::function::IsCallable TFunction>
	requires (std::is_same_v<typename core::function::traits<TFunction>::return_type, void>)
	void manager::ForEach(const std::vector<archetype*>& list, TFunction&& callback) const noexcept
	{
		using function_traits = core::function::traits<TFunction>;
		using argument_types = typename function_traits::argument_types;
		using return_type = typename function_traits::return_type;

		for (const auto& archetype : list)
		{
			const auto& pool = archetype->mPool;

			// Retrieve pointers to components from pool
			auto cache_pointers = [&]<typename... TComponents>(std::tuple<TComponents...>*) constexpr noexcept
			{
				return std::array<std::byte*, sizeof...(TComponents)>
				{
					[&] <typename TComponent>() constexpr noexcept
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
					}.operator() < TComponents > ()
						...
				};
			}(static_cast<argument_types*>(nullptr));

			archetype->AccessGuard([&]()
				{
					for (int i = pool.size(); i > 0; --i)
					{
						[&] <typename... TComponents>(std::tuple<TComponents...>*) constexpr noexcept
						{
							return callback([&]<typename TComponent>() constexpr noexcept -> TComponent
							{
								constexpr auto I = core::types::tuple_type_to_index_v<TComponent, argument_types>;
								auto& pComponent = cache_pointers[I];

								if constexpr (std::is_pointer_v<TComponent>)
								{
									if (pComponent == nullptr) return reinterpret_cast<TComponent>(nullptr);
								}

								auto p = pComponent;
								pComponent += sizeof(core::types::full_decay_t<TComponent>);

								return std::is_pointer_v<TComponent> ? reinterpret_cast<TComponent>(p)
									: reinterpret_cast<TComponent>(*p);
							}.operator() < TComponents > ()
								...);
						}(static_cast<argument_types*>(nullptr));
					}
				});
		}
	}

	template <core::function::IsCallable TFunction>
	requires (std::is_same_v<typename core::function::traits<TFunction>::return_type, bool>)
	void manager::ForEach(const std::vector<archetype*>& list, TFunction&& callback) const noexcept
	{
		using function_traits = core::function::traits<TFunction>;
		using argument_types = typename function_traits::argument_types;
		using return_type = typename function_traits::return_type;

		for (const auto& archetype : list)
		{
			const auto& pool = archetype->mPool;

			// Retrieve pointers to components from pool
			auto cache_pointers = [&]<typename... TComponents>(std::tuple<TComponents...>*) constexpr noexcept
			{
				return std::array<std::byte*, sizeof...(TComponents)>
				{
					[&] <typename TComponent>() constexpr noexcept
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
					}.operator()<TComponents>()
					...
				};
			}(static_cast<argument_types*>(nullptr));

			bool breakLoop = false;

			archetype->AccessGuard([&]()
				{
					for (int i = pool.size(); i > 0; --i)
					{
						bool return_value = [&] <typename... TComponents>(std::tuple<TComponents...>*) constexpr noexcept -> bool
						{
							return callback([&]<typename TComponent>() constexpr noexcept -> TComponent
							{
								constexpr auto I = core::types::tuple_type_to_index_v<TComponent, argument_types>;
								auto& pComponent = cache_pointers[I];

								if constexpr (std::is_pointer_v<TComponent>)
								{
									if (pComponent == nullptr) return reinterpret_cast<TComponent>(nullptr);
								}

								auto p = pComponent;
								pComponent += sizeof(core::types::full_decay_t<TComponent>);

								return std::is_pointer_v<TComponent> ? reinterpret_cast<TComponent>(p)
									: reinterpret_cast<TComponent>(*p);
							}.operator() < TComponents > ()
								...);
						}(static_cast<argument_types*>(nullptr));

						if (return_value == true)
						{
							breakLoop = true;
							break;
						}
					}
				});

			if (breakLoop) break;
		}
	}

	template <typename... TComponents>
	[[nodiscard]] archetype& manager::GetArchetype() noexcept
	{
		if constexpr (((std::is_same_v<TComponents, component::entity> == false) && ...))
		{
			return GetArchetype(GetComponentList<component::entity, TComponents...>());
		}
		else
		{
			return GetArchetype(GetComponentList<TComponents...>());
		}
	}

	[[nodiscard]] archetype& manager::GetArchetype(std::span<const component::info* const> component_list) noexcept
	{
		// Create or get archetype
		core::bitarray<settings::max_component_types> component_bits{};

		for (const auto& component : component_list)
		{
			assert(component->mUID != component::info::INVALID_ID);
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
			mArchetypes.emplace_back(
				std::make_unique<archetype>(
					*this, component_list, component_bits));
			archetype_itr = std::prev(mArchetypes.end());
			mSystemMgr.AddArchetypeToSystems((*archetype_itr).get());
		}

		return **archetype_itr;
	}

	[[nodiscard]] std::vector<archetype*> manager::Search(const query& query)
	{
		std::vector<archetype*> archetypesFound{};

		for (const auto& archetype : mArchetypes)
		{
			if (query.Compare(archetype->GetBits()))
			{
				archetypesFound.push_back(archetype.get());
			}
		}

		return archetypesFound;
	}

	void manager::Run() noexcept
	{
		mSystemMgr.Run();
	}

	[[nodiscard]] const entity_info& manager::GetEntityDetails(const component::entity& entity) const noexcept
	{
		const auto& entityDetail = mEntityInfos[entity.mGlobalID];
		assert(entityDetail.mValidation == entity.mValidation);
		return entityDetail;
	}

	void manager::DeleteEntity(component::entity& entityToDelete) noexcept
	{
		assert(entityToDelete.isZombie() == false);

		auto& entityDetail = GetEntityDetails(entityToDelete);

		assert(entityDetail.mArchetype != nullptr);

		mEntityInfos[entityToDelete.mGlobalID].mValidation.mZombie = true;
		entityDetail.mArchetype->DestroyEntity(entityToDelete);
	}

	void manager::SystemDeleteEntity(const component::entity& deletedEntity, const component::entity& swappedEntity) noexcept
	{
		auto& entityDetail = mEntityInfos[deletedEntity.mGlobalID];

		mEntityInfos[swappedEntity.mGlobalID].mPoolIndex = entityDetail.mPoolIndex;

		SystemDeleteEntity(deletedEntity);
	}

	void manager::SystemDeleteEntity(const component::entity& deletedEntity) noexcept
	{
		auto& entityDetail = mEntityInfos[deletedEntity.mGlobalID];

		++entityDetail.mValidation.mGeneration;
		entityDetail.mValidation.mZombie = false;
		entityDetail.mNextFreeEntity = mNextFreeEntity;
		mNextFreeEntity = static_cast<int>(deletedEntity.mGlobalID);
	}

	template <typename... TComponents>
	inline const auto& manager::GetComponentList() const noexcept
	{
		static constexpr auto component_list = std::array{ &component::info_v<TComponents>... };
		
		return component_list;
	}

	[[nodiscard]] component::entity manager::AllocNewEntity(int poolIndex, archetype& archetype) noexcept
	{
		assert(mNextFreeEntity >= 0);

		const int entityIndex = mNextFreeEntity;
		auto& newEntityInfo = mEntityInfos[entityIndex];

		mNextFreeEntity = newEntityInfo.mNextFreeEntity;

		newEntityInfo.mPoolIndex = poolIndex;
		newEntityInfo.mArchetype = &archetype;

		component::entity newEntity;

		newEntity.mGlobalID = static_cast<uint32_t>(entityIndex);
		newEntity.mValidation = newEntityInfo.mValidation;
		
		return newEntity;
	}
}