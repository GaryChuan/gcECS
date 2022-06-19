/******************************************************************************
filename: vector3.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of a vector3 class which represents
a vector in 2D euclidean space.
******************************************************************************/
#pragma once
#include <cassert>

namespace core::math
{
	struct vector3
	{
		constexpr vector3() : vector3{ 0.f } {}
		constexpr vector3(float _x, float _y, float _z) noexcept : x{ _x }, y{ _y }, z{ _z }{}
		constexpr vector3(float _f) noexcept : x{ _f }, y{ _f }, z{ _f } {}
		constexpr vector3& operator = (const vector3& rhs) noexcept
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			return *this;
		}

		float GetMagnitude() const noexcept
		{
			return std::sqrtf(x * x + y * y + z * z);
		}

		float GetMagnitudeSquared() const noexcept
		{
			return x * x + y * y + z * z;
		}

		void Normalize() noexcept
		{
			float magnitude = GetMagnitude();

			x /= magnitude;
			y /= magnitude;
			z /= magnitude;
		}

		vector3 Normalized() noexcept
		{
			float magnitude = GetMagnitude();

			return { x / magnitude, y / magnitude, z / magnitude };
		}

		constexpr bool operator == (const vector3& rhs) noexcept
		{
			return (x == rhs.x && y == rhs.y && z == rhs.z);
		}

		inline const vector3& operator += (const vector3& rhs) noexcept
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;

			return *this;
		}

		inline const vector3& operator -= (const vector3& rhs) noexcept
		{
			x -= rhs.x;
			y -= rhs.y;

			return *this;
		}

		inline const vector3& operator *= (const vector3& rhs) noexcept
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;

			return *this;
		}

		inline const vector3& operator *= (const float scalar) noexcept
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;

			return *this;
		}

		inline const vector3& operator /= (const float scalar) noexcept
		{
			float div = 1.0f / scalar;

			x *= div;
			y *= div;
			z *= div;

			return *this;
		}

		union
		{
			struct
			{
				float x, y, z;
			};

			struct
			{
				float r, g, b;
			};
		};
		
	};

	constexpr vector3 operator + (const vector3& v1, const vector3& v2) noexcept
	{
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}

	constexpr  vector3 operator - (const vector3& v1) noexcept
	{
		return { -v1.x, -v1.y, -v1.z };
	}

	constexpr  vector3 operator - (const vector3& v1, const vector3& v2) noexcept
	{
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}

	constexpr vector3 operator / (const vector3& v1, float scalar) noexcept
	{
		float div = 1.0f / scalar;
		return { v1.x * div, v1.y * div, v1.z * div };
	}

	constexpr vector3 operator * (const vector3& v1, float scalar) noexcept
	{
		return { v1.x * scalar, v1.y * scalar, v1.z * scalar };
	}

	constexpr vector3 operator * (float scalar, const vector3& v1) noexcept
	{
		return { v1.x * scalar, v1.y * scalar, v1.z * scalar };
	}

	constexpr vector3 operator * (const vector3& v1, const vector3& v2) noexcept
	{
		return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
	}
}
