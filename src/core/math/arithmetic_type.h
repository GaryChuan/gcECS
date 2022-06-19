/******************************************************************************
filename: arithmetic type.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of a templated arithmetic type class
that allows for a base type to inherit from and to carry out basic
arithmetic operations and comparisons.
******************************************************************************/
#pragma once
#include <type_traits>

namespace core::math
{
	template <typename T>
	concept is_arithmetic_type = requires { std::is_arithmetic_v<T>; };

	template <is_arithmetic_type T>
	struct arithmetic_type
	{
	public:
		arithmetic_type() noexcept = default;
		arithmetic_type(const T& t) : mValue{ t } {}

		inline arithmetic_type<T>& operator = (const T& rhs) noexcept
		{
			mValue = rhs;
			return *this;
		}
		inline arithmetic_type<T>& operator += (const T& rhs) noexcept
		{
			mValue += rhs;
			return *this;
		}

		inline arithmetic_type<T>& operator -= (const T& rhs) noexcept
		{
			mValue -= rhs;
			return *this;
		}

		inline arithmetic_type<T>& operator *= (const T& rhs) noexcept
		{
			mValue *= rhs;
			return *this;
		}

		inline arithmetic_type<T>& operator /= (const T& rhs) noexcept
		{
			mValue /= rhs;
			return *this;
		}

		inline operator T() const noexcept
		{
			return mValue;
		}

		inline auto operator <=> (const arithmetic_type<T>& t) const = default;

		template <typename Scalar>
		inline auto operator <=> (const Scalar& s) const 
		{ 
			return mValue <=> s; 
		}

		friend arithmetic_type<T> operator + (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2) noexcept
		{
			return t1.mValue + t2.mValue;
		}

		friend arithmetic_type<T> operator - (const arithmetic_type<T>& t1) noexcept
		{
			return -t1.mValue;
		}

		friend arithmetic_type<T> operator - (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2) noexcept
		{
			return t1.mValue - t2.mValue;
		}

		friend arithmetic_type<T> operator / (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2) noexcept
		{
			return t1.mValue / t2.mValue;
		}

		friend arithmetic_type<T> operator * (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2) noexcept
		{
			return t1.mValue * t2.mValue;
		}

		template <typename Scalar>
		friend arithmetic_type<T> operator / (const arithmetic_type<T>& t1, const Scalar& scalar) noexcept
		{
			return t1.mValue / scalar;
		}
		
		template <typename Scalar>
		friend arithmetic_type<T> operator * (const arithmetic_type<T>& t1, const Scalar& scalar) noexcept
		{
			return t1.mValue * scalar;
		}
		
		template <typename Scalar>
		friend arithmetic_type<T> operator * (const Scalar& scalar, const arithmetic_type<T>& t1) noexcept
		{
			return scalar * t1.mValue;
		}

	protected: 
		T mValue;
	};
}