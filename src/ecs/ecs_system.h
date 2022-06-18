/******************************************************************************
filename: ecs_system.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of ecs systems. It includes the 
implementation of the system manager and the system interfaces.
******************************************************************************/
#pragma once
#include <functional>

namespace ecs
{
	class manager;
}

namespace ecs::system
{
	class manager;

	struct base
	{
	public:
		using query				   = std::tuple<>;
		constexpr static auto name = "unnamed system";

		base(const base&) noexcept = delete;
		
		__inline base(ecs::manager& ecsMgr) noexcept;

		void OnUpdate() noexcept {}
		
	protected:
		ecs::manager& mECSMgr;

	private:
		friend system::manager;

		std::function<void(archetype*)> mTryAddArchetypeFn{};
	};

	template <typename TSystem>
	requires ( std::derived_from<TSystem, system::base> )
	struct derived final : TSystem
	{
		__inline derived(ecs::manager& ecsMgr) noexcept;

		derived(const derived&) = delete;
		
		__inline void Run() noexcept;

	private: 
		friend system::manager;

		__inline void TryAddArchetype(archetype* archetype_ptr)
			requires (&TSystem::OnUpdate == &system::base::OnUpdate);

	private:
		std::vector<archetype*> mArchetypes{};

	};

	class manager
	{
		struct info final
		{
			using Interface = std::unique_ptr<system::base>;
			using Callback = void(*)(system::base&);
			using Destructor = void(*)(std::unique_ptr<base>&);

			Interface mSystem{};
			Callback mCallback{};
			Destructor mDestructor{};

			__inline info(
				Interface&& interface, 
				Callback&& callback, 
				Destructor&& destructor) noexcept;
		};

	public:
		template <typename TSystem>
		requires(std::derived_from<TSystem, system::base>)
		__inline void RegisterSystem(ecs::manager& ecsMgr) noexcept;

		__inline void Run() noexcept;

		__inline ~manager() noexcept;

	private:
		friend ecs::manager;

		__inline void AddArchetypeToSystems(archetype* archetype_ptr) noexcept;

	private:
		std::vector<info> mSystemInfos;
	};
}

#include "details/ecs_system.hpp"