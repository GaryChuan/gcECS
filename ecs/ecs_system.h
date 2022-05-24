#pragma once
#include <memory>
#include <vector>

namespace ecs::system
{
	class base
	{
	};

	template <typename TSystem>
	struct interface : base
	{
	};

	struct info
	{
		std::unique_ptr<interface<system::base>> mInterface;
	};

	class manager
	{
	public:
		template <typename TSystem>
		void RegisterSystem()
		{
			/*mSystemInfos.emplace_back(
				info
				{
					.mInterface = std::make_unique<interface<TSystem>>()
				}
			);*/
		}

	private:
		std::vector<info> mSystemInfos;
	};
}