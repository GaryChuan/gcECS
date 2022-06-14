#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include "../core/functions.h"

namespace ecs
{
	class manager;
}

namespace ecs::system
{
	struct base
	{
	public:
		base(const base&) noexcept = delete;
		base(ecs::manager& ecsMgr) noexcept
			: mECSMgr { ecsMgr }
		{
		}

	protected:
		ecs::manager& mECSMgr;
	};

	template <typename TSystem>
	requires ( std::derived_from<TSystem, system::base> )
	struct derived final : TSystem
	{
		std::vector<archetype*> mArchetypes{};

		derived(ecs::manager& ecsMgr) :
			TSystem{ ecsMgr },
			mArchetypes { TSystem::mECSMgr.Search(*this) }
		{
		}

		derived(const derived&) = delete;

		void Run() noexcept
		{
			TSystem::mECSMgr.for_each(mArchetypes, *this);
		}
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
			mSystems.emplace_back(
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
		}

		void Run() noexcept
		{
			for (info& system : mSystems)
			{
				system.mCallback(*system.mInterface);
			}
		}

		~manager()
		{
			for (info& system : mSystems)
			{
				system.mDestructor(system.mInterface);
			}
		}
	private:
		std::vector<info> mSystems;
	};
}