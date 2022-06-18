/******************************************************************************
filename: ecs_component.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains details of ecs::component::info containing infos of 
component types. It also contains a struct type that serves for verifying
user defined component types. The component manager and entity component is 
also defined here.
******************************************************************************/
#pragma once
#include <memory>
#include "types.h"

namespace ecs::component
{
	struct info final
	{
		constexpr static std::uint16_t INVALID_ID = 0xffff;

		using ConstructFn = void (*)(std::byte*) noexcept;
		using DestructFn  = void (*)(std::byte*) noexcept;
		using MoveFn	  = void (*)(std::byte*, std::byte*) noexcept;

		mutable std::uint16_t mUID{};
		std::uint32_t mSize{};

		ConstructFn mConstructFn{ nullptr };
		DestructFn  mDestructFn{ nullptr };
		MoveFn		mMoveFn{ nullptr };
	};

	struct type
	{
		struct data
		{
			const char* mName;
		};
	};

	template <typename T>
	concept is_valid_type = requires 
	{ 
		core::types::full_decay_t<T>::typedef_v; 
	};

	namespace detail
	{
		template <typename T>
		inline consteval info CreateInfo() noexcept;

		template <typename T>
		struct info_detail final
		{
			static constexpr auto value = CreateInfo<T>();
		};
	}

	template <typename TComponent>
	constexpr auto& info_v = detail::info_detail<core::types::full_decay_t<TComponent>>::value;

	union entity final
	{
		static constexpr auto typedef_v = type::data{ .mName = "entity" };

		union validation final
		{
			std::uint32_t mValue;

			struct
			{
				std::uint32_t mGeneration : 31
							, mZombie : 1;
			};

			constexpr bool operator == (const validation& rhs) const noexcept { return mValue == rhs.mValue; }
		};
		static_assert(sizeof(validation) == sizeof(std::uint32_t), "entity::validation is not 32 bytes!");

		struct
		{
			std::uint32_t mGlobalID;
			validation mValidation;
		};

		constexpr bool isZombie(void)					   const noexcept { return mValidation.mZombie; }
		constexpr bool operator == (const entity& rhs)     const noexcept { return mValue == rhs.mValue; }

		std::uint64_t mValue{};
	};
	static_assert(sizeof(entity) == sizeof(std::uint64_t), "entity is not 64 bytes!");

	class manager final
	{
	public:
		manager() = default;
		manager(const manager&) = delete;

		template <ecs::component::is_valid_type TComponent>
		inline constexpr void RegisterComponent() const noexcept;

	private:
		inline static std::uint32_t mUniqueID = 0;
	};
}