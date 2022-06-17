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

		inline std::size_t Size() const noexcept
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

		/*inline void SetBit(unsigned bitIndex) noexcept;
		inline std::size_t Size() const noexcept;
		inline bool GetBit(unsigned bitIndex) const noexcept;
		inline bool Compare(const bitarray& rhs) noexcept;
		inline bool Compare(const bitarray& rhs) const noexcept;

		inline BitChunkType operator[](std::size_t index) const noexcept;*/
	private:
		std::array<BitChunkType, N / sizeof(BitChunkType) + 1> mBits{};
	};
}