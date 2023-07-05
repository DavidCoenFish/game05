//--------------------------------------------------------------------------------------
// File: GraphicsMemory.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "CommonPCH.h"
#include "Common/DirectXTK12/GraphicsMemory.h"
#include "Common/DirectXTK12/LinearAllocator.h"
#include "Common/Log/log.h"

//using namespace DirectX;
//using Microsoft::WRL::ComPtr;
//using std::lock_guard< std::mutex > = std::lock_guard<std::mutex>;

static const size_t MinPageSize = 64 * 1024;
static const size_t MinAllocSize = 4 * 1024;
static const size_t AllocatorIndexShift = 12; // start block sizes at 4KB
static const size_t AllocatorPoolCount = 21; // allocation sizes up to 2GB supported
static const size_t PoolIndexScale = 1; // multiply the allocation size this amount to push large values into the next bucket

static_assert((1 << AllocatorIndexShift) == MinAllocSize, "1 << AllocatorIndexShift must == MinPageSize (in KiB)");
static_assert((MinPageSize & (MinPageSize - 1)) == 0, "MinPageSize size must be a power of 2");
static_assert((MinAllocSize & (MinAllocSize - 1)) == 0, "MinAllocSize size must be a power of 2");
static_assert(MinAllocSize >= (4 * 1024), "MinAllocSize size must be greater than 4K");

inline size_t NextPow2(size_t x) noexcept
{
   x--;
   x |= x >> 1;
   x |= x >> 2;
   x |= x >> 4;
   x |= x >> 8;
   x |= x >> 16;
#ifdef _WIN64
   x |= x >> 32;
#endif
   return ++x;
}

inline size_t GetPoolIndexFromSize(size_t x) noexcept
{
   size_t allocatorPageSize = x >> AllocatorIndexShift;
   // gives a value from range:
   // 0 - sub-4k allocator
   // 1 - 4k allocator
   // 2 - 8k allocator
   // 4 - 16k allocator
   // etc...
   // Need to convert to an index.
   DWORD bitIndex = 0;
#ifdef _WIN64
   return _BitScanForward64(&bitIndex, allocatorPageSize) ? bitIndex + 1 : 0;
#else
   return _BitScanForward(&bitIndex, static_cast<DWORD>(allocatorPageSize)) ? bitIndex + 1 : 0;
#endif
}

inline size_t GetPageSizeFromPoolIndex(size_t x) noexcept
{
   x = (x == 0) ? 0 : x - 1; // clamp to zero
   return std::max<size_t>(MinPageSize, size_t(1) << (x + AllocatorIndexShift));
}

//--------------------------------------------------------------------------------------
// DeviceAllocator : honors memory requests associated with a particular device
//--------------------------------------------------------------------------------------
class DeviceAllocator
{
public:
   DeviceAllocator(_In_ ID3D12Device* device) noexcept(false)
      : mDevice(device)
   {
      if (!device)
            throw std::invalid_argument("Invalid device parameter");

      for (size_t i = 0; i < mPools.size(); ++i)
      {
            size_t pageSize = GetPageSizeFromPoolIndex(i);
            mPools[i] = std::make_unique<DirectX::LinearAllocator>(
               mDevice.Get(),
               pageSize);
      }
   }

   DeviceAllocator(DeviceAllocator&&) = delete;
   DeviceAllocator& operator= (DeviceAllocator&&) = delete;

   DeviceAllocator(DeviceAllocator const&) = delete;
   DeviceAllocator& operator= (DeviceAllocator const&) = delete;

   // Explicitly destroy LinearAllocators inside a critical section
   ~DeviceAllocator()
   {
      std::lock_guard< std::mutex > lock(mMutex);

      for (auto& allocator : mPools)
      {
            allocator.reset();
      }
   }

   DirectX::GraphicsResource Alloc(_In_ size_t size, _In_ size_t alignment)
   {
      std::lock_guard< std::mutex > lock(mMutex);

      // Which memory pool does it live in?
      size_t poolSize = NextPow2((alignment + size) * PoolIndexScale);
      size_t poolIndex = GetPoolIndexFromSize(poolSize);
      assert(poolIndex < mPools.size());

      // If the allocator isn't initialized yet, do so now
      auto& allocator = mPools[poolIndex];
      assert(allocator != nullptr);
      assert(poolSize < MinPageSize || poolSize == allocator->PageSize());

      auto page = allocator->FindPageForAlloc(size, alignment);
      if (!page)
      {
            LOG_MESSAGE_WARNING("GraphicsMemory failed to allocate page (%zu requested bytes, %zu alignment)\n", size, alignment);
            throw std::bad_alloc();
      }

      size_t offset = page->Suballocate(size, alignment);

      // Return the information to the user
      return DirectX::GraphicsResource(
            page,
            page->GpuAddress() + offset,
            page->UploadResource(),
            static_cast<BYTE*>(page->BaseMemory()) + offset,
            offset,
            size);
   }

   // Submit page fences to the command queue
   void KickFences(_In_ ID3D12CommandQueue* commandQueue)
   {
      std::lock_guard< std::mutex > lock(mMutex);

      for (auto& i : mPools)
      {
            if (i)
            {
               i->RetirePendingPages();
               i->FenceCommittedPages(commandQueue);
            }
      }
   }

   void GarbageCollect()
   {
      std::lock_guard< std::mutex > lock(mMutex);

      for (auto& i : mPools)
      {
            if (i)
            {
               i->Shrink();
            }
      }
   }

   void GetStatistics(DirectX::GraphicsMemoryStatistics& stats) const
   {
      size_t totalPageCount = 0;
      size_t committedMemoryUsage = 0;
      size_t totalMemoryUsage = 0;
           
      std::lock_guard< std::mutex > lock(mMutex);

      for (auto& i : mPools)
      {
            if (i)
            {
               totalPageCount += i->TotalPageCount();
               committedMemoryUsage += i->CommittedMemoryUsage();
               totalMemoryUsage += i->TotalMemoryUsage();
            }
      }

      stats = {};
      stats.committedMemory = committedMemoryUsage;
      stats.totalMemory = totalMemoryUsage;
      stats.totalPages = totalPageCount;
   }

#if !(defined(_XBOX_ONE) && defined(_TITLE)) && !defined(_GAMING_XBOX)
   ID3D12Device* GetDevice() const noexcept { return mDevice.Get(); }
#endif

private:
   Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
   std::array<std::unique_ptr<DirectX::LinearAllocator>, AllocatorPoolCount> mPools;
   mutable std::mutex mMutex;
};


//--------------------------------------------------------------------------------------
// GraphicsMemory::Impl
//--------------------------------------------------------------------------------------

class DirectX::GraphicsMemory::Impl
{
public:
    Impl(GraphicsMemory* owner) noexcept(false)
        : mOwner(owner)
        , m_peakCommited(0)
        , m_peakBytes(0)
        , m_peakPages(0)
    {
    #if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
        if (s_graphicsMemory)
        {
            throw std::exception("GraphicsMemory is a singleton");
        }

        s_graphicsMemory = this;
    #endif
    }

    Impl(Impl&&) = default;
    Impl& operator= (Impl&&) = default;

    Impl(Impl const&) = delete;
    Impl& operator= (Impl const&) = delete;

    ~Impl()
    {
        mDeviceAllocator.reset();
    }

    void Initialize(_In_ ID3D12Device* device)
    {
        mDeviceAllocator = std::make_unique<DeviceAllocator>(device);
    }

    GraphicsResource Allocate(size_t size, size_t alignment)
    {
        return mDeviceAllocator->Alloc(size, alignment);
    }

    void Commit(_In_ ID3D12CommandQueue* commandQueue)
    {
        mDeviceAllocator->KickFences(commandQueue);
    }

    void GarbageCollect()
    {
        mDeviceAllocator->GarbageCollect();
    }

    void GetStatistics(GraphicsMemoryStatistics& stats)
    {
        mDeviceAllocator->GetStatistics(stats);

        if (stats.committedMemory > m_peakCommited)
        {
            m_peakCommited = stats.committedMemory;
        }
        stats.peakCommitedMemory = m_peakCommited;

        if (stats.totalMemory > m_peakBytes)
        {
            m_peakBytes = stats.totalMemory;
        }
        stats.peakTotalMemory = m_peakBytes;

        if (stats.totalPages > m_peakPages)
        {
            m_peakPages = stats.totalPages;
        }
        stats.peakTotalPages = m_peakPages;
    }

    void ResetStatistics()
    {
        m_peakCommited = 0;
        m_peakBytes = 0;
        m_peakPages = 0;
}

    GraphicsMemory* mOwner;

private:
    std::unique_ptr<DeviceAllocator> mDeviceAllocator;

    size_t  m_peakCommited;
    size_t  m_peakBytes;
    size_t  m_peakPages;
};


//--------------------------------------------------------------------------------------
// GraphicsMemory
//--------------------------------------------------------------------------------------

// Public constructor.
DirectX::GraphicsMemory::GraphicsMemory(_In_ ID3D12Device* device)
    : pImpl(std::make_unique<Impl>(this))
{
    pImpl->Initialize(device);
}


// Move constructor.
DirectX::GraphicsMemory::GraphicsMemory(DirectX::GraphicsMemory&& moveFrom) noexcept
    : pImpl(std::move(moveFrom.pImpl))
{
    pImpl->mOwner = this;
}


// Move assignment.
DirectX::GraphicsMemory& DirectX::GraphicsMemory::operator= (DirectX::GraphicsMemory&& moveFrom) noexcept
{ 
    pImpl = std::move(moveFrom.pImpl);
    pImpl->mOwner = this;
    return *this;
}


// Public destructor.
DirectX::GraphicsMemory::~GraphicsMemory()
{
}


DirectX::GraphicsResource DirectX::GraphicsMemory::Allocate(size_t size, size_t alignment)
{
    assert(alignment >= 4); // Should use at least DWORD alignment
    return pImpl->Allocate(size, alignment);
}


void DirectX::GraphicsMemory::Commit(_In_ ID3D12CommandQueue* commandQueue)
{
    pImpl->Commit(commandQueue);
}


void DirectX::GraphicsMemory::GarbageCollect()
{
    pImpl->GarbageCollect();
}

DirectX::GraphicsMemoryStatistics DirectX::GraphicsMemory::GetStatistics()
{
    GraphicsMemoryStatistics stats;
    pImpl->GetStatistics(stats);
    return stats;
}

void DirectX::GraphicsMemory::ResetStatistics()
{
    pImpl->ResetStatistics();
}

/*
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
GraphicsMemory& GraphicsMemory::Get(_In_opt_ ID3D12Device*)
{
    if (!Impl::s_graphicsMemory || !Impl::s_graphicsMemory->mOwner)
        throw std::exception("GraphicsMemory singleton not created");

    return *Impl::s_graphicsMemory->mOwner;
}
#else
DirectX::GraphicsMemory& DirectX::GraphicsMemory::Get(_In_opt_ ID3D12Device* device)
{
    if (Impl::s_graphicsMemory.empty())
        throw std::exception("GraphicsMemory singleton not created");

    std::map<ID3D12Device*, DirectX::GraphicsMemory::Impl*>::const_iterator it;
    if (!device)
    {
        // Should only use nullptr for device for single GPU usage
        assert(Impl::s_graphicsMemory.size() == 1);

        it = Impl::s_graphicsMemory.cbegin();
    }
    else
    {
        it = Impl::s_graphicsMemory.find(device);
    }

    if (it == Impl::s_graphicsMemory.cend() || !it->second->mOwner)
        throw std::exception("GraphicsMemory per-device singleton not created");

    return *it->second->mOwner;
}
#endif
*/

//--------------------------------------------------------------------------------------
// GraphicsResource smart-pointer interface
//--------------------------------------------------------------------------------------

DirectX::GraphicsResource::GraphicsResource() noexcept
    : mPage(nullptr)
    , mGpuAddress {}
    , mResource(nullptr)
    , mMemory(nullptr)
    , mBufferOffset(0)
    , mSize(0)
{
}

DirectX::GraphicsResource::GraphicsResource(
    _In_ LinearAllocatorPage* page,
    _In_ D3D12_GPU_VIRTUAL_ADDRESS gpuAddress,
    _In_ ID3D12Resource* resource,
    _In_ void* memory,
    _In_ size_t offset,
    _In_ size_t size) noexcept
    : mPage(page)
    , mGpuAddress(gpuAddress)
    , mResource(resource)
    , mMemory(memory)
    , mBufferOffset(offset)
    , mSize(size)
{
    assert(mPage != nullptr);
    mPage->AddRef();
}

DirectX::GraphicsResource::GraphicsResource(DirectX::GraphicsResource&& other) noexcept
    : mPage(nullptr)
    , mGpuAddress{}
    , mResource(nullptr)
    , mMemory(nullptr)
    , mBufferOffset(0)
    , mSize(0)
{
    Reset(std::move(other));
}

DirectX::GraphicsResource::~GraphicsResource()
{
    if (mPage)
    {
        mPage->Release();
        mPage = nullptr;
    }
}

DirectX::GraphicsResource&& DirectX::GraphicsResource::operator= (DirectX::GraphicsResource&& other) noexcept
{
    Reset(std::move(other));
    return std::move(*this);
}

void DirectX::GraphicsResource::Reset() noexcept
{
    if (mPage)
    {
        mPage->Release();
        mPage = nullptr;
    }

    mGpuAddress = {};
    mResource = nullptr;
    mMemory = nullptr;
    mBufferOffset = 0;
    mSize = 0;
}

void DirectX::GraphicsResource::Reset(DirectX::GraphicsResource&& alloc) noexcept
{
    if (mPage)
    {
        mPage->Release();
        mPage = nullptr;
    }

    mGpuAddress = alloc.GpuAddress();
    mResource = alloc.Resource();
    mMemory = alloc.Memory();
    mBufferOffset = alloc.ResourceOffset();
    mSize = alloc.Size();
    mPage = alloc.mPage;

    alloc.mGpuAddress = {};
    alloc.mResource = nullptr;
    alloc.mMemory = nullptr;
    alloc.mBufferOffset = 0;
    alloc.mSize = 0;
    alloc.mPage = nullptr;
}


//--------------------------------------------------------------------------------------
// SharedGraphicsResource
//--------------------------------------------------------------------------------------

DirectX::SharedGraphicsResource::SharedGraphicsResource() noexcept
    : mSharedResource(nullptr)
{
}

DirectX::SharedGraphicsResource::SharedGraphicsResource(DirectX::GraphicsResource&& resource) noexcept(false)
    : mSharedResource(std::make_shared<DirectX::GraphicsResource>(std::move(resource)))
{
}

DirectX::SharedGraphicsResource::SharedGraphicsResource(DirectX::SharedGraphicsResource&& resource) noexcept
    : mSharedResource(std::move(resource.mSharedResource))
{
}

DirectX::SharedGraphicsResource::SharedGraphicsResource(const DirectX::SharedGraphicsResource& resource) noexcept
    : mSharedResource(resource.mSharedResource)
{
}

DirectX::SharedGraphicsResource::~SharedGraphicsResource()
{
}

DirectX::SharedGraphicsResource&& DirectX::SharedGraphicsResource::operator= (DirectX::SharedGraphicsResource&& resource) noexcept
{
    mSharedResource = std::move(resource.mSharedResource);
    return std::move(*this);
}

DirectX::SharedGraphicsResource&& DirectX::SharedGraphicsResource::operator= (DirectX::GraphicsResource&& resource)
{
    mSharedResource = std::make_shared<GraphicsResource>(std::move(resource));
    return std::move(*this);
}

DirectX::SharedGraphicsResource& DirectX::SharedGraphicsResource::operator= (const DirectX::SharedGraphicsResource& resource) noexcept
{
    mSharedResource = resource.mSharedResource;
    return *this;
}

void DirectX::SharedGraphicsResource::Reset() noexcept
{
    mSharedResource.reset();
}

void DirectX::SharedGraphicsResource::Reset(DirectX::GraphicsResource&& resource)
{
    mSharedResource = std::make_shared<GraphicsResource>(std::move(resource));
}

void DirectX::SharedGraphicsResource::Reset(DirectX::SharedGraphicsResource&& resource) noexcept
{
    mSharedResource = std::move(resource.mSharedResource);
}

void DirectX::SharedGraphicsResource::Reset(const DirectX::SharedGraphicsResource& resource) noexcept
{
    mSharedResource = resource.mSharedResource;
}

