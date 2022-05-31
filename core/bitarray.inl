#include "bitarray.h"

namespace core
{
	template<std::size_t N, typename BitChunkType>
	void BitArray<N, BitChunkType>::SetBit(unsigned bitIndex) noexcept
	{
		unsigned I = bitIndex / sizeof(BitChunkType);
		unsigned J = bitIndex % sizeof(BitChunkType);

		mBits[I] |= (static_cast<BitChunkType>(1) << J);
	}

	template<std::size_t N, typename BitChunkType>
	bool BitArray<N, BitChunkType>::GetBit(unsigned bitIndex) noexcept
	{
		unsigned I = bitIndex / sizeof(BitChunkType);
		unsigned J = bitIndex % sizeof(BitChunkType);

		return mBits[I] & (static_cast<BitChunkType>(1) << J);
	}

	template<std::size_t N, typename BitChunkType>
	bool Compare(const BitArray<N, BitChunkType>& rhs)
	{
		return mBits == rhs.mBits;
	}
}