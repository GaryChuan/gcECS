#include "bitarray.h"

namespace core
{
	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	void BitArray<N, BitChunkType>::SetBit(unsigned bitIndex) noexcept
	{
		unsigned I = bitIndex / sizeof(BitChunkType);
		unsigned J = bitIndex % sizeof(BitChunkType);

		mBits[I] |= (static_cast<BitChunkType>(1) << J);
	}

	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	std::size_t BitArray<N, BitChunkType>::Size() const noexcept
	{
		return mBits.size();
	}

	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	BitChunkType BitArray<N, BitChunkType>::operator[](std::size_t index) const noexcept
	{
		assert(index < mBits.size());
		return mBits[index];
	}

	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	bool BitArray<N, BitChunkType>::GetBit(unsigned bitIndex) noexcept
	{
		unsigned I = bitIndex / sizeof(BitChunkType);
		unsigned J = bitIndex % sizeof(BitChunkType);

		return mBits[I] & (static_cast<BitChunkType>(1) << J);
	}

	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	bool BitArray<N, BitChunkType>::Compare(const BitArray<N, BitChunkType>& rhs) const noexcept
	{
		return mBits == rhs.mBits;
	}
}