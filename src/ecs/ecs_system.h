#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <concepts>
#include <functional>
#include "../core/functions.h"

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
		base(ecs::manager& ecsMgr) noexcept
			: mECSMgr { ecsMgr }
		{
		}

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
		derived(ecs::manager& ecsMgr) :
			TSystem{ ecsMgr }
		{
		}

		derived(const derived&) = delete;
		
		void Run() noexcept
		{
			if constexpr (&TSystem::OnUpdate == &system::base::OnUpdate)
			{
				TSystem::mECSMgr.for_each(mArchetypes, *this);
			}
			else
			{
				TSystem::OnUpdate();
			}
		}

	private: 
		friend system::manager;

		void TryAddArchetype(archetype* archetype_ptr) 
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

			info(Interface&& interface, Callback&& callback, Destructor&& destructor)
				: mSystem		{ std::move(interface) }
				, mCallback		{ callback }
				, mDestructor	{ destructor }
			{
			}
		};

	public:
		template <typename TSystem>
		requires( std::derived_from<TSystem, system::base> )
		void RegisterSystem(ecs::manager& ecsMgr)
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

		void Run() noexcept
		{
			for (info& system : mSystemInfos)
			{
				system.mCallback(*system.mSystem);
			}
		}

		~manager()
		{
			for (info& system : mSystemInfos)
			{
				system.mDestructor(system.mSystem);
			}
		}

	private:
		friend ecs::manager;

		void AddArchetypeToSystems(archetype* archetype_ptr)
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

	private:
		std::vector<info> mSystemInfos;
	};
}