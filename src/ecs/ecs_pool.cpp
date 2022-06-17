#include "ecs_pool.h"
#include <cassert>
#include <Windows.h>
#include <memoryapi.h>

namespace ecs
{
	pool::~pool()
	{
		Clear();

		for (int i = 0; i < static_cast<int>(mInfos.size()); ++i)
		{
			VirtualFree(mComponents[i], 0, MEM_RELEASE);
		}
	}

	void pool::Initialize(std::span<const component::info* const> infos) noexcept
	{
		mInfos = std::move(infos);

		// Reserve memory for each component
		for (int i = 0; i < static_cast<int>(mInfos.size()); ++i)
		{
			const auto& info = *mInfos[i];

			assert(info.mSize <= settings::virtual_page_size);

			const auto nPage = 1 + GetPageFromIndex(info, settings::max_entities_per_pool);
			mComponents[i] = reinterpret_cast<std::byte*>(VirtualAlloc(nullptr, nPage * settings::virtual_page_size,
														  MEM_RESERVE, PAGE_NOACCESS));

			assert(mComponents[i]);
		}
	}

	int pool::Append() noexcept
	{
		assert(mSize < (settings::max_entities - 1));

		for (int i = 0; i < static_cast<int>(mInfos.size()); ++i)
		{
			const auto& info	= *mInfos[i];
			const auto nextPageIndex = GetPageFromIndex(info, mSize + 1);

			if (GetPageFromIndex(info, mSize) != nextPageIndex)
			{
				auto pNewPage = mComponents[i] + nextPageIndex * settings::virtual_page_size;
				auto pData = VirtualAlloc(pNewPage, settings::virtual_page_size,
										  MEM_COMMIT, PAGE_READWRITE);


				assert(reinterpret_cast<std::byte*>(pData) == pNewPage);
								
			}
			if (info.mConstructFn)
			{
				info.mConstructFn(mComponents[i] + mSize * info.mSize);
			}
		}

		return mSize++;
	}

	void pool::Delete(std::uint32_t index) noexcept
	{
		assert(index < mSize);
		assert(index >= 0);

		// Reduce entity count
		--mSize;

		for (int i = 0; i < static_cast<int>(mInfos.size()); ++i)
		{
			const auto& info	= *mInfos[i];
			auto pComponent		= mComponents[i];
			const auto lastPageIndex = GetPageFromIndex(info, mSize + 1);

			if (index == mSize && info.mMoveFn)
			{
				info.mMoveFn(&pComponent[index * info.mSize], &pComponent[mSize * info.mSize]);
			}
			else
			{
				if (info.mDestructFn)
				{
					info.mDestructFn(&pComponent[mSize * info.mSize]);
					memcpy(&pComponent[index * info.mSize], &pComponent[mSize * info.mSize], info.mSize);
				}
			}

			// Free last page if no longer occupied
			if (GetPageFromIndex(info, mSize) != lastPageIndex)
			{
				auto pData = &pComponent[lastPageIndex * settings::virtual_page_size];
				auto pFree = VirtualFree(pData, settings::virtual_page_size, MEM_DECOMMIT);

				assert(pFree);
			}
		}
	}

	constexpr std::uint32_t pool::Size() const noexcept
	{
		return mSize;
	}

	void pool::Clear() noexcept
	{
		while (mSize)
		{
			Delete(mSize - 1);
		}
	}

	const unsigned pool::GetPageFromIndex(
		const component::info& info,
		int entityIndex) const noexcept
	{
		return ((info.mSize * entityIndex) - 1) / settings::virtual_page_size;
	}

	const int pool::FindComponentIndexFromUID(const std::uint16_t UID) const noexcept
	{
		for (int i = 0; i < static_cast<int>(mInfos.size()); ++i)
		{
			if (mInfos[i]->mUID == UID) return i;
		}

		return -1;
	}
}