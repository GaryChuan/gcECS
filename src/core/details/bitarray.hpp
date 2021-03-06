/******************************************************************************
filename: bitarray.hpp
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of bitarray which serves as a custom
implementation of std::bitset.
******************************************************************************/
#pragma once

namespace core
{
	template <std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	void bitarray<N, BitChunkType>::SetBit(unsigned bitIndex) noexcept
	{
		unsigned I = bitIndex / sizeof(BitChunkType);
		unsigned J = bitIndex % sizeof(BitChunkType);

		mBits[I] |= (static_cast<BitChunkType>(1) << J);
	}

	template <std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	std::size_t bitarray<N, BitChunkType>::size() const noexcept
	{
		return mBits.size();
	}
	
	template <std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	BitChunkType bitarray<N, BitChunkType>::operator[](std::size_t index) const noexcept
	{
		assert(index < mBits.size());
		return mBits[index];
	}

	template <std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	bool bitarray<N, BitChunkType>::GetBit(unsigned bitIndex) const noexcept
	{
		unsigned I = bitIndex / sizeof(BitChunkType);
		unsigned J = bitIndex % sizeof(BitChunkType);

		return mBits[I] & (static_cast<BitChunkType>(1) << J);
	}

	template <std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	bool bitarray<N, BitChunkType>::Compare(const bitarray<N, BitChunkType>& rhs) const noexcept
	{
		return mBits == rhs.mBits;
	}
}