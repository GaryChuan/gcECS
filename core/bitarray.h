#pragma once
#include <array>
#include <cassert>
#include <type_traits>

namespace core
{
	template <std::size_t N, typename BitChunkType = uint64_t>
	requires ( std::is_trivial_v<BitChunkType> )
	class BitArray final
	{
	public:
		void SetBit(unsigned bitIndex) noexcept;
		std::size_t Size() const noexcept;
		bool GetBit(unsigned bitIndex) noexcept;
		bool Compare(const BitArray& rhs) const noexcept;

		BitChunkType operator[](std::size_t index) const noexcept;
	private:
		std::array<BitChunkType, N / sizeof(BitChunkType) + 1> mBits;
	};
	
}