#pragma once
#include <span>
#include <array>
#include "ecs_component.h"
#include "ecs_settings.h"

namespace ecs
{
	class pool
	{
	public:
		pool() noexcept = default;
		pool(const pool&) noexcept = delete;
		~pool();

		void Initialize(std::span<const component::info* const> infos) noexcept;
		void Clear() noexcept;
		int Append() noexcept;
		void Delete(std::uint32_t index) noexcept;
		
		constexpr std::uint32_t Size() const noexcept;

		template <typename T>
		requires (std::is_same_v<T, std::decay_t<T>>)
		T& GetComponent(const std::uint32_t entityIndex) const noexcept;

	private:
		const unsigned GetPageFromIndex(
			const component::info& info, 
			int entityIndex) const noexcept;

		const int FindComponentIndexFromUID(const std::uint16_t UID) const noexcept;

	private:
		std::span<const component::info* const> mInfos{};
		std::array<std::byte*, settings::max_component_per_entities> mComponents{};
		std::uint32_t mSize{}; // entity count
	};
}