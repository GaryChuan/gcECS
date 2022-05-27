#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include "../core/function_traits.h"

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
	struct interface final : TSystem
	{
		interface(ecs::manager& ecsMgr) :
			TSystem{ ecsMgr }
		{
		}

		interface(const interface&) = delete;

		void Run() noexcept
		{
			TSystem::mECSMgr.for_each(*this);
		}
	};

	class manager
	{
		struct info final
		{
			using Interface = std::unique_ptr<system::base>;
			using Callback = void(*)(system::base&);

			Interface mInterface{};
			Callback mCallback{};
		};

	public:
		template <typename TSystem>
		requires( std::derived_from<TSystem, system::base> )
		void RegisterSystem(ecs::manager& ecsMgr)
		{
			mSystems.emplace_back(
				info
				{
					.mInterface = std::make_unique<system::interface<TSystem>>(ecsMgr),
					.mCallback = [](system::base& system)
					{
						static_cast<system::interface<TSystem>&>(system).Run();
					}
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

	private:
		std::vector<info> mSystems;
	};
}