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

		std::function<void(archetype*)> mAddArchetypeFn{};
	};

	template <typename TSystem>
	concept HasOnUpdate = requires { &TSystem::OnUpdate == &base::OnUpdate; };

	template <typename TSystem>
	requires ( std::derived_from<TSystem, system::base> )
	struct derived final : TSystem
	{
		derived(ecs::manager& ecsMgr) :
			TSystem{ ecsMgr }
		{
		}

		derived(const derived&) = delete;
		
		void Run() noexcept requires (!HasOnUpdate<TSystem> )
		{
			TSystem::OnUpdate();
		}

		void Run() noexcept requires (HasOnUpdate<TSystem>)
		{
			TSystem::mECSMgr.for_each(mArchetypes, *this);
		}

	private: 
		friend system::manager;

		void AddArchetype(archetype* archetype_ptr) requires (HasOnUpdate<TSystem>)
		{
			assert(archetype_ptr != nullptr);
			mArchetypes.push_back(archetype_ptr);
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

			Interface mInterface{};
			Callback mCallback{};
			Destructor mDestructor{};

			info(Interface&& interface, Callback&& callback, Destructor&& destructor)
				: mInterface{ std::move(interface) }
				, mCallback { callback }
				, mDestructor { destructor }
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

			systemInfo.mInterface->mAddArchetypeFn = std::bind(
				&derived<TSystem>::AddArchetype,
				static_cast<derived<TSystem>*>(systemInfo.mInterface.get()),
				std::placeholders::_1);
		}

		void Run() noexcept
		{
			for (info& system : mSystemInfos)
			{
				system.mCallback(*system.mInterface);
			}
		}

		~manager()
		{
			for (info& system : mSystemInfos)
			{
				system.mDestructor(system.mInterface);
			}
		}

	private:
		friend ecs::manager;

		void AddArchetypeToSystems(archetype* archetype_ptr)
		{
			for (auto& info : mSystemInfos)
			{
				if (info.mInterface->mAddArchetypeFn != nullptr)
				{
					info.mInterface->mAddArchetypeFn(archetype_ptr);
				}
			}
		}

	private:
		std::vector<info> mSystemInfos;
	};
}