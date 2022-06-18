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

		inline T& operator /= (const T& rhs) noexcept
		{
			mValue /= rhs;
			return *this;
		}

		inline auto operator <=> (const arithmetic_type<T>& t) const = default;

		template <typename Scalar>
		inline auto operator <=> (const Scalar& s) const 
		{ 
			return mValue <=> s; 
		}

		friend arithmetic_type<T> operator + (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2) noexcept;
		friend arithmetic_type<T> operator - (const arithmetic_type<T>& t1) noexcept;
		friend arithmetic_type<T> operator - (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2) noexcept;

		template <typename Scalar>
		friend arithmetic_type<T> operator / (const arithmetic_type<T>& t1, Scalar scalar) noexcept;
		
		template <typename Scalar>
		friend arithmetic_type<T> operator * (const arithmetic_type<T>& t1, Scalar scalar) noexcept;
		
		template <typename Scalar>
		friend arithmetic_type<T> operator * (Scalar scalar, const arithmetic_type<T>& t1) noexcept;

		friend arithmetic_type<T> operator * (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2) noexcept;

	protected: 
		T mValue;
	};

	template <typename T>
	arithmetic_type<T> operator + (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2)
	{
		return t1.mValue + t2.mValue;
	}

	template <typename T>
	arithmetic_type<T> operator - (const arithmetic_type<T>& t1) noexcept
	{
		return -t1.mValue;
	}
	
	template <typename T>
	arithmetic_type<T> operator - (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2) noexcept
	{
		return t1.mValue - t2.mValue;
	}

	template <typename T, typename Scalar>
	arithmetic_type<T> operator / (const arithmetic_type<T>& t1, Scalar scalar) noexcept
	{
		return t1.mValue / scalar;
	}

	template <typename T, typename Scalar>
	arithmetic_type<T> operator * (const arithmetic_type<T>& t1, Scalar scalar) noexcept
	{
		return t1.mValue * scalar;
	}

	template <typename T, typename Scalar>
	arithmetic_type<T> operator * (Scalar scalar, const arithmetic_type<T>& t1) noexcept
	{
		return t1.mValue * scalar;
	}

	template <typename T>
	arithmetic_type<T> operator * (const arithmetic_type<T>& t1, const arithmetic_type<T>& t2) noexcept
	{
		return t1.mValue * t2.mValue;
	}
}