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

namespace ecs::system
{
	class manager;

	struct base
	{
	public:
		using query = std::tuple<>;
		constexpr static auto name = "unnamed system";

		base(const base&) noexcept = delete;
		
		inline base(ecs::manager& ecsMgr) noexcept;

		void OnUpdate() noexcept {}
		
	protected:
		ecs::manager& mECSMgr;

	private:
		friend system::manager;

		std::function<void(archetype*)> mTryAddArchetypeFn{};
	};

	namespace details
	{
		template <typename TSystem>
		requires (std::derived_from<TSystem, system::base>)
			struct derived final : TSystem
		{
			inline derived(ecs::manager& ecsMgr) noexcept;
			derived(const derived&) = delete;

			inline void Run() noexcept;

		private:
			friend system::manager;

			inline void TryAddArchetype(archetype* archetype_ptr)
				requires (&TSystem::OnUpdate == &system::base::OnUpdate);

		private:
			std::vector<archetype*> mArchetypes{};

		};
	}

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

			inline info(
				Interface&& interface, 
				Callback&& callback, 
				Destructor&& destructor) noexcept;
		};

	public:
		template <typename TSystem>
		requires(std::derived_from<TSystem, system::base>)
		inline void RegisterSystem(ecs::manager& ecsMgr) noexcept;

		inline void Run() noexcept;

		inline ~manager() noexcept;

	private:
		friend ecs::manager;

		inline void AddArchetypeToSystems(archetype* archetype_ptr) noexcept;

	private:
		std::vector<info> mSystemInfos;
	};
}