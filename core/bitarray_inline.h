#include "bitarray.h"

namespace core
{
	/*template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	inline void bitarray<N, BitChunkType>::SetBit(unsigned bitIndex) noexcept
	{
		unsigned I = bitIndex / sizeof(BitChunkType);
		unsigned J = bitIndex % sizeof(BitChunkType);

		mBits[I] |= (static_cast<BitChunkType>(1) << J);
	}

	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	inline std::size_t bitarray<N, BitChunkType>::Size() const noexcept
	{
		return mBits.size();
	}

	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	inline BitChunkType bitarray<N, BitChunkType>::operator[](std::size_t index) const noexcept
	{
		assert(index < mBits.size());
		return mBits[index];
	}

	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	inline bool bitarray<N, BitChunkType>::GetBit(unsigned bitIndex) const noexcept
	{
		unsigned I = bitIndex / sizeof(BitChunkType);
		unsigned J = bitIndex % sizeof(BitChunkType);

		return mBits[I] & (static_cast<BitChunkType>(1) << J);
	}

	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	inline bool bitarray<N, BitChunkType>::Compare(const bitarray<N, BitChunkType>& rhs) noexcept
	{
		return mBits == rhs.mBits;
	}

	template<std::size_t N, typename BitChunkType>
	requires (std::is_trivial_v<BitChunkType>)
	inline bool bitarray<N, BitChunkType>::Compare(const bitarray<N, BitChunkType>& rhs) const noexcept
	{
		return mBits == rhs.mBits;
	}*/
}