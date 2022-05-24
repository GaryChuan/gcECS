#pragma once
#include <memory>
#include <vector>

namespace ecs::system
{
	template <typename TSystem>
	struct interface
	{

		TSystem mSystem;
	};
	
	class system
	{

	};

	struct info
	{
		std::unique_ptr<interface<system>> mInterface;
	};

	class manager
	{
	public:
		template <typename... TSystems>
		void RegisterSystems()
		{
			(RegisterSystem<TSystems>(), ...);
		}

	private:
		template <typename TSystem>
		void RegisterSystem()
		{
			mSystemInfos.emplace_back(std::make_unique<interface<TSystem>>());
		}

	private:
		std::vector<info> mSystemInfos;
	};
}