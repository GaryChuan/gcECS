#pragma once
#include <array>
#include <type_traits>

namespace core
{
	template <std::size_t N, typename BitChunkType = uint64_t>
	requires ( std::is_trivial_v<BitChunkType> )
	class BitArray final
	{
	public:
		void SetBit(unsigned bitIndex) noexcept;
		bool GetBit(unsigned bitIndex) noexcept;
		bool Compare(const BitArray& rhs) const noexcept;

	private:
		std::array<BitChunkType, N / sizeof(BitChunkType) + 1> mBits;
	};
	
}