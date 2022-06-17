#pragma once
#include <span>
#include "functions.h"
#include "bitarray.h"
#include "ecs_pool.h"

namespace ecs
{
	class manager;

	class archetype
	{
	public:
		using bits = core::bitarray<settings::max_component_types>;

		// archetype(manager& ecsMgr) noexcept;
		archetype() noexcept = default;
		archetype(const archetype&) noexcept = delete;

		[[nodiscard]] const bits& GetBits() const noexcept;
		[[nodiscard]] bool CompareBits(const bits& bits) const noexcept;

	private:
		friend manager;

		void Initialize(std::span<const component::info* const> component_list);
		void Initialize(std::span<const component::info* const> component_list,
						const bits& component_bits);

		template <typename TComponent>
		const TComponent& GetComponent(const std::uint32_t entityIndex) const noexcept
		{
			return mPool.GetComponent<TComponent>(entityIndex);
		}

		bits mComponentBits{};
		pool mPool{};
	};
}