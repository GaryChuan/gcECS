#pragma once
#include <cstdint>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace ecs::component
{
	struct info final
	{
		constexpr static std::uint16_t invalid_id = 0xffff;

		using ConstructFn = void (*)(std::byte*) noexcept;
		using DestructFn  = void (*)(std::byte*) noexcept;
		using MoveFn	  = void (*)(std::byte*, std::byte*) noexcept;

		mutable std::uint16_t mUID{};
		std::uint32_t mSize{};

		ConstructFn mConstructFn{};
		DestructFn  mDestructFn{};
		MoveFn		mMoveFn{};
	};

	namespace detail
	{
		template <typename T>
		consteval info CreateInfo() noexcept
		{
			constexpr auto constructFn = [](std::byte* p) noexcept { new(p) T{}; };
			constexpr auto destructFn = [](std::byte* p) noexcept { std::destroy_at(reinterpret_cast<T*>(p)); };
			constexpr auto moveFn = [](std::byte* p1, std::byte* p2) noexcept { *reinterpret_cast<T*>(p1) = std::move(*reinterpret_cast<T*>(p2)); };

			return info
			{
				.mUID			= info::invalid_id,
				.mSize			= sizeof(T),
				.mConstructFn	= std::is_trivially_constructible_v<T> ? nullptr : constructFn,
				.mDestructFn	= std::is_trivially_destructible_v<T> ? nullptr : destructFn,
				.mMoveFn		= std::is_trivially_move_assignable_v<T> ? nullptr : moveFn
			};
		}

		template <typename T>
		static constexpr auto info_v = CreateInfo<T>();
	}

	template <typename TComponent>
	constexpr auto& info_v = detail::info_v<std::decay_t<TComponent>>;

	union entity final
	{
		union validation final
		{
			std::uint32_t mValue;

			struct
			{
				std::uint32_t mGenerator : 31
							, mZombie : 1;
			};
		};

		struct
		{
			std::uint32_t mGlobalID;
			validation mValidation;
		};

		std::uint64_t mValue;
	};

	class manager final
	{
	public:
		manager() = default;
		manager(const manager&) = delete;

		template <typename TComponent>
		requires ( std::is_same_v<TComponent, std::decay_t<TComponent>> )
		constexpr void RegisterComponent() const noexcept
		{
			if constexpr (info_v<TComponent>.mUID == info::invalid_id)
			{
				info_v<TComponent>.mUID = mUniqueID++;
			}
		}

	private:
		inline static std::uint32_t mUniqueID;
	};
}