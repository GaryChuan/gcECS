#pragma once
#include <span>
#include <array>
#include "ecs_component.h"
#include "ecs_settings.h"

namespace ecs
{
	class manager;

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
		
		std::uint32_t size() const noexcept;

		template <typename TComponent>
		requires (std::is_same_v<TComponent, std::decay_t<TComponent>>)
		TComponent& GetComponent(const std::uint32_t entityIndex) noexcept
		{
			const auto componentIndex = FindComponentIndexFromUID(component::info_v<TComponent>.mUID);
			return *reinterpret_cast<TComponent*>(&mComponents[componentIndex][entityIndex * sizeof(TComponent)]);
		}

		template <typename TComponent>
		requires (std::is_same_v<TComponent, std::decay_t<TComponent>>)
		const TComponent& GetComponent(const std::uint32_t entityIndex) const noexcept
		{
			const auto componentIndex = FindComponentIndexFromUID(component::info_v<TComponent>.mUID);
			return *reinterpret_cast<TComponent*>(&mComponents[componentIndex][entityIndex * sizeof(TComponent)]);
		}

	private:
		friend manager;

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