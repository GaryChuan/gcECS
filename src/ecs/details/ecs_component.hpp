#pragma once

namespace ecs::component
{
	namespace detail
	{
		template <typename T>
		consteval info CreateInfo() noexcept
		{
			constexpr auto constructFn = [](std::byte* p) noexcept { new(p) T; };
			constexpr auto destructFn = [](std::byte* p) noexcept { std::destroy_at(reinterpret_cast<T*>(p)); };
			constexpr auto moveFn = [](std::byte* p1, std::byte* p2) noexcept { *reinterpret_cast<T*>(p1) = std::move(*reinterpret_cast<T*>(p2)); };

			return info
			{
				.mUID = info::invalid_id,
				.mSize = static_cast<uint32_t>(sizeof(T)),
				.mConstructFn = std::is_trivially_constructible_v<T> ? nullptr : constructFn,
				.mDestructFn = std::is_trivially_destructible_v<T> ? nullptr : destructFn,
				.mMoveFn = std::is_trivially_move_assignable_v<T> ? nullptr : moveFn
			};
		}
	}

	template <ecs::component::is_valid_type TComponent>
	constexpr void manager::RegisterComponent() const noexcept
	{
		if constexpr (info_v<TComponent>.mUID == info::invalid_id)
		{
			info_v<TComponent>.mUID = mUniqueID++;
		}
	}
}