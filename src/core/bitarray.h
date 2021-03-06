/******************************************************************************
filename: bitarray.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the declaration of bitarray which serves as a custom
implementation of std::bitset.
******************************************************************************/
#pragma once
#include <array>
#include <cassert>
#include <type_traits>

namespace core
{
	template <std::size_t N, typename BitChunkType = uint64_t>
	requires ( std::is_trivial_v<BitChunkType> )
	class bitarray final
	{
	public:

		inline void SetBit(unsigned bitIndex) noexcept;

		inline std::size_t size() const noexcept;

		inline BitChunkType operator[](std::size_t index) const noexcept;

		inline bool GetBit(unsigned bitIndex) const noexcept;

		inline bool Compare(const bitarray<N, BitChunkType>& rhs) const noexcept;

	private:
		std::array<BitChunkType, N / sizeof(BitChunkType) + 1> mBits{};
	};
}

#include "details/bitarray.hpp"