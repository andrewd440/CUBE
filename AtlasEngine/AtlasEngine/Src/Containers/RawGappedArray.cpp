#include "Containers\RawGappedArray.h"
#include "Memory\MemoryUtil.h"

#include <algorithm>

FTypelessPageArray::FTypelessPageArray()
	: mElementSize(0)
	, mAlignment(1)
	, mPageSize(0)
	, mActiveCount(0)
	, mNextFreePage(0)
{
}

FTypelessPageArray::~FTypelessPageArray()
{
	DeleteAllPages();
}

void FTypelessPageArray::Init(const uint32_t PageSize, const uint32_t ElementSize, const uint32_t Alignment)
{
	ASSERT(Alignment != 0 && PageSize != 0 && ElementSize != 0);

	mElementSize = ElementSize;
	mAlignment = Alignment;
	mPageSize = PageSize;

	DeleteAllPages();
	AddPage();
	mNextFreePage = 0;
}

uint32_t FTypelessPageArray::Allocate()
{
	if (mPages[mNextFreePage].DeadList.empty())
	{
		AssignNextFreePage();
	}
	
	// Get the first free element
	Page& FreePage = mPages[mNextFreePage];

	const uint32_t FreeElement = FreePage.DeadList.top();
	FreePage.DeadList.pop();

   	const uint32_t FreeElementGlobalIndex = FreeElement + mPageSize * mNextFreePage;

	// Find the first element that is greater than the newly allocated index and insert before this position
	const auto ActivePosition = std::find_if(FreePage.ActiveList.begin(), FreePage.ActiveList.end(), [FreeElement](const uint32_t Index){ return Index > FreeElement; });

	FreePage.ActiveList.insert(ActivePosition, FreeElement);

	mActiveCount++;
	return FreeElementGlobalIndex;
}

void FTypelessPageArray::AssignNextFreePage()
{
	const uint32_t PageCount = mPages.size();
	for (uint32_t i = mNextFreePage + 1; i < PageCount; i++)
	{
		if (!mPages[i].DeadList.empty())
		{
			mNextFreePage = i;
			return;
		}
	}

	// All pages are full, add a new one
	AddPage();
	mNextFreePage = PageCount;
}

void FTypelessPageArray::Free(const uint32_t Index)
{
	const uint32_t PageID = Index / mPageSize;
	const uint32_t ElementID = Index % mPageSize;

	Page& ElementPage = mPages[PageID];
	auto ActiveIndex = std::find(ElementPage.ActiveList.begin(), ElementPage.ActiveList.end(), ElementID);
	ASSERT(ActiveIndex != ElementPage.ActiveList.end() && "Trying to free an inactive element.");

	ElementPage.ActiveList.erase(ActiveIndex);

	ElementPage.DeadList.push(Index);

	if (PageID < mNextFreePage)
	{
		mNextFreePage = PageID;
	}

	mActiveCount--;
}

void FTypelessPageArray::AddPage()
{
	mPages.push_back(Page{});
	Page& NewPage = mPages.back();

	NewPage.Data = (uint8_t*)FMemory::AllocateAligned(std::max(mPageSize, mAlignment) * mElementSize, mAlignment);

	// Fill the dead list with every element
	for (uint32_t i = 0; i < mPageSize; i++)
	{
		NewPage.DeadList.push(i);
	}
}

void FTypelessPageArray::DeleteAllPages()
{
	for (auto& Page : mPages)
	{
		if (Page.Data)
		{
			FMemory::FreeAligned(Page.Data);
		}
	}

	mPages.clear();
}
