// --------------------------------------------------------------------------------------
// File: GraphicsMemory.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// Http://go.microsoft.com/fwlink/?LinkID=615561
// --------------------------------------------------------------------------------------
#include "common/common_pch.h"

#include "common/direct_xtk12/graphics_memory.h"
#include "common/direct_xtk12/linear_allocator.h"
#include "common/log/log.h"

// Using namespace DirectX;
// Using Microsoft::WRL::ComPtr;
// Using std::lock_guard< std::mutex > = std::lock_guard<std::mutex>;
static const size_t MinPageSize = 64* 1024;
static const size_t MinAllocSize = 4* 1024;
static const size_t AllocatorIndexShift = 12;
// Start block sizes at 4KB
static const size_t AllocatorPoolCount = 21;
// Allocation sizes up to 2GB supported
static const size_t PoolIndexScale = 1;
// Multiply the allocation size this amount to push large values into the next bucket
static_assert (
    (1 << AllocatorIndexShift) == MinAllocSize,
    "1 << AllocatorIndexShift must == MinPageSize (in KiB)"
    );
static_assert (
    (MinPageSize& (MinPageSize - 1)) == 0,
    "MinPageSize size must be a power of 2"
    );
static_assert (
    (MinAllocSize& (MinAllocSize - 1)) == 0,
    "MinAllocSize size must be a power of 2"
    );
static_assert (
    MinAllocSize >= (4* 1024),
    "MinAllocSize size must be greater than 4K"
    );
inline size_t NextPow2(size_t in_x) noexcept
{
    in_x --;
    in_x |= in_x >> 1;
    in_x |= in_x >> 2;
    in_x |= in_x >> 4;
    in_x |= in_x >> 8;
    in_x |= in_x >> 16;
#ifdef _WIN64
        in_x |= in_x >> 32;
#endif

    return++ in_x;
}

inline size_t GetPoolIndexFromSize(size_t in_x) noexcept
{
    size_t allocator_page_size = in_x >> AllocatorIndexShift;
    // Gives a value from range:
    // 0 - sub-4k allocator
    // 1 - 4k allocator
    // 2 - 8k allocator
    // 4 - 16k allocator
    // Etc...
    // Need to convert to an index.
    DWORD bit_index = 0;
#ifdef _WIN64
        return bit_scan_forward64(
            &bit_index,
            allocator_page_size
            ) ? bit_index + 1 : 0;
#else
        return bit_scan_forward(
            &bit_index,
            static_cast < DWORD > (allocator_page_size)
            ) ? bit_index + 1 : 0;
#endif

}

inline size_t GetPageSizeFromPoolIndex(size_t in_x) noexcept
{
    in_x = (in_x == 0) ? 0 : in_x - 1;
    // Clamp to zero
    return std::max < size_t > (
        MinPageSize,
        size_t(1) << (in_x + AllocatorIndexShift)
        );
}

// --------------------------------------------------------------------------------------
// DeviceAllocator : honors memory requests associated with a particular device
// --------------------------------------------------------------------------------------
class DeviceAllocator
{
public:
    DeviceAllocator(in_in_ ID3D12Device* in_device) noexcept (false) 
        : _device(in_device)
    {
        if (!in_device) throw std::invalid_argument("Invalid device parameter");
        for (size_t i = 0; i < _pools.size();++ i)
        {
            size_t page_size = GetPageSizeFromPoolIndex(i);
            _pools[i] = std::make_unique < DirectX::LinearAllocator > (
                _device.Get(),
                page_size
                );
        }
    }

    DeviceAllocator(DeviceAllocator &&) = delete;
    DeviceAllocator&_operator=(DeviceAllocator &&) = delete;
    DeviceAllocator(DeviceAllocator const&) = delete;
    DeviceAllocator&_operator=(DeviceAllocator const&) = delete;
    // Explicitly destroy LinearAllocators inside a critical section
    ~DeviceAllocator()
    {
        std::lock_guard < std::mutex > lock(_mutex);
        for (auto&allocator : _pools)
        {
            allocator.reset();
        }
    }

    DirectX::GraphicsResource Alloc(
        in_in_ size_t in_size,
        in_in_ size_t in_alignment
        )
    {
        std::lock_guard < std::mutex > lock(_mutex);
        // Which memory pool does it live in?
        size_t pool_size = NextPow2((in_alignment + in_size) * PoolIndexScale);
        size_t pool_index = GetPoolIndexFromSize(pool_size);
        assert(pool_index < _pools.in_size());
        // If the allocator isn't initialized yet, do so now
        auto&allocator = _pools[pool_index];
        assert(allocator != nullptr);
        assert(pool_size < MinPageSize || pool_size == allocator->PageSize());
        auto page = allocator->FindPageForAlloc(
            in_size,
            in_alignment
            );
        if (!page)
        {
            LOG_MESSAGE_WARNING(
                "GraphicsMemory failed to allocate page (%zu requested bytes, %zu alignment)\n",
                in_size,
                in_alignment
                );
            throw std::bad_alloc();
        }
        size_t offset = page->Suballocate(
            in_size,
            in_alignment
            );
        // Return the information to the user
        return DirectX::GraphicsResource(
            page,
            page->GpuAddress() + offset,
            page->UploadResource(),
            static_cast < BYTE* > (page->BaseMemory()) + offset,
            offset,
            in_size
            );
    }

    // Submit page fences to the command queue
    void KickFences(in_in_ ID3D12CommandQueue* in_command_queue)
    {
        std::lock_guard < std::mutex > lock(_mutex);
        for (auto&i : _pools)
        {
            if (i)
            {
                i->RetirePendingPages();
                i->FenceCommittedPages(in_command_queue);
            }
        }
    }

    void GarbageCollect()
    {
        std::lock_guard < std::mutex > lock(_mutex);
        for (auto&i : _pools)
        {
            if (i)
            {
                i->Shrink();
            }
        }
    }

    void GetStatistics(DirectX::GraphicsMemoryStatistics&in_stats) const
    {
        size_t total_page_count = 0;
        size_t committed_memory_usage = 0;
        size_t total_memory_usage = 0;
        std::lock_guard < std::mutex > lock(_mutex);
        for (auto&i : _pools)
        {
            if (i)
            {
                total_page_count += i->TotalPageCount();
                committed_memory_usage += i->CommittedMemoryUsage();
                total_memory_usage += i->TotalMemoryUsage();
            }
        }
        in_stats = {};
        in_stats._committed_memory = committed_memory_usage;
        in_stats._total_memory = total_memory_usage;
        in_stats._total_pages = total_page_count;
    }


private:
    Microsoft::WRL::ComPtr < ID3D12Device > _device;
    std::_array < std::_unique_ptr < DirectX::LinearAllocator >, AllocatorPoolCount > _pools;
    mutable std::_mutex _mutex;
#if ! (defined (_XBOX_ONE) && defined (_TITLE)) && !defined (_GAMING_XBOX)
        ID3D12Device* GetDevice() const noexcept
        {
            return _device.Get();
        }

#endif

};
// --------------------------------------------------------------------------------------
// GraphicsMemory::Impl
// --------------------------------------------------------------------------------------
class DirectX::GraphicsMemory::Impl
{
public:
    Impl(GraphicsMemory* in_owner) noexcept (false) 
        : _owner(in_owner)
        , _peak_commited(0)
        , _peak_bytes(0)
        , _peak_pages(0)
    {
#if(defined (_XBOX_ONE) && defined (_TITLE)) || defined (_GAMING_XBOX)
            if (s_graphics_memory)
            {
                throw std::exception("GraphicsMemory is a singleton");
            }
            s_graphics_memory = this;
#endif

    }

    Impl(Impl &&) = default;
    Impl&_operator=(Impl &&) = default;
    Impl(Impl const&) = delete;
    Impl&_operator=(Impl const&) = delete;
    ~Impl()
    {
        _device_allocator.reset();
    }

    void Initialize(in_in_ ID3D12Device* in_device)
    {
        _device_allocator = std::make_unique < DeviceAllocator > (in_device);
    }

    GraphicsResource Allocate(
        size_t in_size,
        size_t in_alignment
        )
    {
        return _device_allocator->Alloc(
            in_size,
            in_alignment
            );
    }

    void Commit(in_in_ ID3D12CommandQueue* in_command_queue)
    {
        _device_allocator->KickFences(in_command_queue);
    }

    void GarbageCollect()
    {
        _device_allocator->GarbageCollect();
    }

    void GetStatistics(GraphicsMemoryStatistics&in_stats)
    {
        _device_allocator->GetStatistics(in_stats);
        if (in_stats._committed_memory > _peak_commited)
        {
            _peak_commited = in_stats._committed_memory;
        }
        in_stats._peak_commited_memory = _peak_commited;
        if (in_stats._total_memory > _peak_bytes)
        {
            _peak_bytes = in_stats._total_memory;
        }
        in_stats._peak_total_memory = _peak_bytes;
        if (in_stats._total_pages > _peak_pages)
        {
            _peak_pages = in_stats._total_pages;
        }
        in_stats._peak_total_pages = _peak_pages;
    }

    void ResetStatistics()
    {
        _peak_commited = 0;
        _peak_bytes = 0;
        _peak_pages = 0;
    }


public:
    GraphicsMemory* _owner;

private:
    std::_unique_ptr < DeviceAllocator > _device_allocator;
    size_t _peak_commited;
    size_t _peak_bytes;
    size_t _peak_pages;
};
// --------------------------------------------------------------------------------------
// GraphicsMemory
// --------------------------------------------------------------------------------------
// Public constructor.
DirectX::GraphicsMemory::GraphicsMemory(in_in_ ID3D12Device* in_device) 
    : impl(std::make_unique < Impl > (this))
{
    impl->Initialize(in_device);
}

// Move constructor.
DirectX::GraphicsMemory::GraphicsMemory(DirectX::GraphicsMemory && in_move_from) noexcept 
    : impl(std::move(in_move_from.impl))
{
    impl->_owner = this;
}

// Move assignment.
DirectX::GraphicsMemory&DirectX::GraphicsMemory::operator=(DirectX::GraphicsMemory && in_move_from) noexcept
{
    impl = std::move(in_move_from.impl);
    impl->_owner = this;
    return* this;
}

// Public destructor.
DirectX::GraphicsMemory::~GraphicsMemory(){}

DirectX::GraphicsResource DirectX::GraphicsMemory::Allocate(
    size_t in_size,
    size_t in_alignment
    )
{
    assert(in_alignment >= 4);
    // Should use at least DWORD alignment
    return impl->Allocate(
        in_size,
        in_alignment
        );
}

void DirectX::GraphicsMemory::Commit(in_in_ ID3D12CommandQueue* in_command_queue)
{
    impl->Commit(in_command_queue);
}

void DirectX::GraphicsMemory::GarbageCollect()
{
    impl->GarbageCollect();
}

DirectX::GraphicsMemoryStatistics DirectX::GraphicsMemory::GetStatistics()
{
    GraphicsMemoryStatistics stats;
    impl->GetStatistics(stats);
    return stats;
}

void DirectX::GraphicsMemory::ResetStatistics()
{
    impl->ResetStatistics();
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
// --------------------------------------------------------------------------------------
// GraphicsResource smart-pointer interface
// --------------------------------------------------------------------------------------
DirectX::GraphicsResource::GraphicsResource() noexcept 
    : _page(nullptr)
    , _gpu_address {}
    , _resource(nullptr)
    , _memory(nullptr)
    , _buffer_offset(0)
    , _size(0){}

DirectX::GraphicsResource::GraphicsResource(
    in_in_ LinearAllocatorPage* in_page,
    in_in_ D3D12_GPU_VIRTUAL_ADDRESS in_gpu_address,
    in_in_ ID3D12Resource* in_resource,
    in_in_ void* in_memory,
    in_in_ size_t in_offset,
    in_in_ size_t in_size
    ) noexcept 
    : _page(in_page)
    , _gpu_address(in_gpu_address)
    , _resource(in_resource)
    , _memory(in_memory)
    , _buffer_offset(in_offset)
    , _size(in_size)
{
    assert(_page != nullptr);
    _page->AddRef();
}

DirectX::GraphicsResource::GraphicsResource(DirectX::GraphicsResource && in_other) noexcept 
    : _page(nullptr)
    , _gpu_address{}
    , _resource(nullptr)
    , _memory(nullptr)
    , _buffer_offset(0)
    , _size(0)
{
    Reset(std::move(in_other));
}

DirectX::GraphicsResource::~GraphicsResource()
{
    if (_page)
    {
        _page->Release();
        _page = nullptr;
    }
}

DirectX::GraphicsResource && DirectX::GraphicsResource::operator=(DirectX::GraphicsResource && in_other) noexcept
{
    Reset(std::move(in_other));
    return std::move(* this);
}

void DirectX::GraphicsResource::Reset() noexcept
{
    if (_page)
    {
        _page->Release();
        _page = nullptr;
    }
    _gpu_address = {};
    _resource = nullptr;
    _memory = nullptr;
    _buffer_offset = 0;
    _size = 0;
}

void DirectX::GraphicsResource::Reset(DirectX::GraphicsResource && in_alloc) noexcept
{
    if (_page)
    {
        _page->Release();
        _page = nullptr;
    }
    _gpu_address = in_alloc.GpuAddress();
    _resource = in_alloc.Resource();
    _memory = in_alloc.Memory();
    _buffer_offset = in_alloc.ResourceOffset();
    _size = in_alloc.Size();
    _page = in_alloc._page;
    in_alloc._gpu_address = {};
    in_alloc._resource = nullptr;
    in_alloc._memory = nullptr;
    in_alloc._buffer_offset = 0;
    in_alloc._size = 0;
    in_alloc._page = nullptr;
}

// --------------------------------------------------------------------------------------
// SharedGraphicsResource
// --------------------------------------------------------------------------------------
DirectX::SharedGraphicsResource::SharedGraphicsResource() noexcept 
    : _shared_resource(nullptr){}

DirectX::SharedGraphicsResource::SharedGraphicsResource(DirectX::GraphicsResource && in_resource) noexcept (false) 
    : _shared_resource(std::make_shared < DirectX::GraphicsResource > (std::move(in_resource))){}

DirectX::SharedGraphicsResource::SharedGraphicsResource(DirectX::SharedGraphicsResource && in_resource) noexcept 
    : _shared_resource(std::move(in_resource._shared_resource)){}

DirectX::SharedGraphicsResource::SharedGraphicsResource(const DirectX::SharedGraphicsResource&in_resource) noexcept 
    : _shared_resource(in_resource._shared_resource){}

DirectX::SharedGraphicsResource::~SharedGraphicsResource(){}

DirectX::SharedGraphicsResource && DirectX::SharedGraphicsResource::operator=(DirectX::SharedGraphicsResource && 
    in_resource) noexcept
{
    _shared_resource = std::move(in_resource._shared_resource);
    return std::move(* this);
}

DirectX::SharedGraphicsResource && DirectX::SharedGraphicsResource::operator=(DirectX::GraphicsResource && in_resource)
{
    _shared_resource = std::make_shared < GraphicsResource > (std::move(in_resource));
    return std::move(* this);
}

DirectX::SharedGraphicsResource&DirectX::SharedGraphicsResource::operator=(const DirectX::SharedGraphicsResource&
    in_resource) noexcept
{
    _shared_resource = in_resource._shared_resource;
    return* this;
}

void DirectX::SharedGraphicsResource::Reset() noexcept
{
    _shared_resource.reset();
}

void DirectX::SharedGraphicsResource::Reset(DirectX::GraphicsResource && in_resource)
{
    _shared_resource = std::make_shared < GraphicsResource > (std::move(in_resource));
}

void DirectX::SharedGraphicsResource::Reset(DirectX::SharedGraphicsResource && in_resource) noexcept
{
    _shared_resource = std::move(in_resource._shared_resource);
}

void DirectX::SharedGraphicsResource::Reset(const DirectX::SharedGraphicsResource&in_resource) noexcept
{
    _shared_resource = in_resource._shared_resource;
}

