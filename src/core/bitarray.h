/******************************************************************************
filename: bitarray.h
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of bitarray which serves as a custom
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

		inline void SetBit(unsigned bitIndex) noexcept
		{
			unsigned I = bitIndex / sizeof(BitChunkType);
			unsigned J = bitIndex % sizeof(BitChunkType);

			mBits[I] |= (static_cast<BitChunkType>(1) << J);
		}

		inline std::size_t size() const noexcept
		{
			return mBits.size();
		}

		inline BitChunkType operator[](std::size_t index) const noexcept
		{
			assert(index < mBits.size());
			return mBits[index];
		}

		inline bool GetBit(unsigned bitIndex) const noexcept
		{
			unsigned I = bitIndex / sizeof(BitChunkType);
			unsigned J = bitIndex % sizeof(BitChunkType);

			return mBits[I] & (static_cast<BitChunkType>(1) << J);
		}

		inline bool Compare(const bitarray<N, BitChunkType>& rhs) noexcept
		{
			return mBits == rhs.mBits;
		}

		inline bool Compare(const bitarray<N, BitChunkType>& rhs) const noexcept
		{
			return mBits == rhs.mBits;
		}

	private:
		std::array<BitChunkType, N / sizeof(BitChunkType) + 1> mBits{};
	};
}