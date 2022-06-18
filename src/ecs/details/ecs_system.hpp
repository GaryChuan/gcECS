/******************************************************************************
filename: ecs_system.hpp
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs system.
******************************************************************************/
#pragma once

namespace ecs::system
{
	base::base(ecs::manager& ecsMgr) noexcept
		: mECSMgr{ ecsMgr }
	{
	}

	template <typename TSystem>
	requires (std::derived_from<TSystem, system::base>)
	derived<TSystem>::derived(ecs::manager& ecsMgr) noexcept:
		TSystem{ ecsMgr }
	{
	}

	template <typename TSystem>
	requires (std::derived_from<TSystem, system::base>)
	void derived<TSystem>::Run() noexcept
	{
		if constexpr (&TSystem::OnUpdate == &system::base::OnUpdate)
		{
			TSystem::mECSMgr.ForEach(mArchetypes, *this);
		}
		else
		{
			TSystem::OnUpdate();
		}
	}

	template <typename TSystem>
	requires (std::derived_from<TSystem, system::base>)
	void derived<TSystem>::TryAddArchetype(archetype* archetype_ptr)
		requires (&TSystem::OnUpdate == &system::base::OnUpdate)
	{
		assert(archetype_ptr != nullptr);

		query query{ *this };

		query.Set(static_cast<TSystem::query*>(nullptr));

		if (query.Compare(archetype_ptr->GetBits()))
		{
			mArchetypes.push_back(archetype_ptr);
		}
	}

	manager::info::info(
		Interface&& interface,
		Callback&& callback,
		Destructor&& destructor) noexcept
		: mSystem{ std::move(interface) }
		, mCallback{ callback }
		, mDestructor{ destructor }
	{
	}

	template <typename TSystem>
	requires(std::derived_from<TSystem, system::base>)
	void manager::RegisterSystem(ecs::manager& ecsMgr) noexcept
	{
		auto& systemInfo = mSystemInfos.emplace_back(
			std::make_unique<system::derived<TSystem>>(ecsMgr),
			[](system::base& system)
			{
				static_cast<system::derived<TSystem>&>(system).Run();
			},
			[](std::unique_ptr<base>& base)
			{
				delete static_cast<system::derived<TSystem>*>(base.release());
			}
			);

		if constexpr (&TSystem::OnUpdate == &system::base::OnUpdate)
		{
			systemInfo.mSystem->mTryAddArchetypeFn = std::bind(
				&derived<TSystem>::TryAddArchetype,
				static_cast<derived<TSystem>*>(systemInfo.mSystem.get()),
				std::placeholders::_1);
		}
	}

	void manager::Run() noexcept
	{
		for (info& system : mSystemInfos)
		{
			system.mCallback(*system.mSystem);
		}
	}

	manager::~manager() noexcept
	{
		for (info& system : mSystemInfos)
		{
			system.mDestructor(system.mSystem);
		}
	}

	void manager::AddArchetypeToSystems(archetype* archetype_ptr) noexcept
	{
		for (auto& info : mSystemInfos)
		{
			auto& system = info.mSystem;

			if (system->mTryAddArchetypeFn != nullptr)
			{
				system->mTryAddArchetypeFn(archetype_ptr);
			}
		}
	}
}