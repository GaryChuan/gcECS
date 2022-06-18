/******************************************************************************
filename: vector2.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of a vector2 class which represents
a vector in 2D euclidean space.
******************************************************************************/
#pragma once
#include <cassert>

namespace core::math
{
	struct vector2
	{
		constexpr vector2() noexcept = default;
		constexpr vector2(float _x, float _y) noexcept : x{ _x }, y{ _y }{}
		constexpr vector2(float _f) noexcept : x{ _f }, y{ _f }{}

		float GetMagnitude() const noexcept
		{
			return std::sqrtf(x * x + y * y);
		}

		float GetMagnitudeSquared() const noexcept
		{
			return x * x + y * y;
		}
		
		void Normalize() noexcept
		{
			float magnitude = GetMagnitude();

			x /= magnitude;
			y /= magnitude;
		}

		vector2 Normalized() noexcept
		{
			float magnitude = GetMagnitude();

			return { x / magnitude, y / magnitude };
		}

		constexpr bool operator == (const vector2& rhs) noexcept
		{
			return (x == rhs.x && y == rhs.y);
		}

		__inline const vector2& operator += (const vector2& rhs) noexcept
		{
			x += rhs.x;
			y += rhs.y;

			return *this;
		}

		__inline const vector2& operator -= (const vector2& rhs) noexcept
		{
			x -= rhs.x;
			y -= rhs.y;

			return *this;
		}

		__inline const vector2& operator *= (const vector2& rhs) noexcept
		{
			x *= rhs.x;
			y *= rhs.y;

			return *this;
		}

		__inline const vector2& operator *= (const float scalar) noexcept
		{
			x *= scalar;
			y *= scalar;

			return *this;
		}

		__inline const vector2& operator /= (const float scalar) noexcept
		{
			float div = 1.0f / scalar;

			x *= div;
			y *= div;

			return *this;
		}

		float x = 0;
		float y = 0;
	};

	constexpr vector2 operator + (const vector2& v1, const vector2& v2) noexcept
	{
		return { v1.x + v2.x, v1.y + v2.y };
	}

	constexpr  vector2 operator - (const vector2& v1) noexcept
	{
		return { -v1.x, -v1.y };
	}

	constexpr  vector2 operator - (const vector2& v1, const vector2& v2) noexcept
	{
		return { v1.x - v2.x, v1.y - v2.y };
	}

	constexpr vector2 operator / (const vector2& v1, float scalar) noexcept
	{
		float div = 1.0f / scalar;

		return { v1.x * div, v1.y * div };
	}

	constexpr vector2 operator * (const vector2& v1, float scalar) noexcept
	{
		return { v1.x * scalar, v1.y * scalar };
	}

	constexpr vector2 operator * (float scalar, const vector2& v1) noexcept
	{
		return { v1.x * scalar, v1.y * scalar };
	}

	constexpr vector2 operator * (const vector2& v1, const vector2& v2) noexcept
	{
		return { v1.x * v2.x, v1.y * v2.y };
	}
}
