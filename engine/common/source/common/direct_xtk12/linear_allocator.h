// --------------------------------------------------------------------------------------
// LinearAllocator.h
//
// A linear allocator. When Allocate is called it will try to return you a pointer into
// Existing graphics memory. If there is no space left from what is allocated, more
// Pages are allocated on-the-fly.
//
// Each allocation must be smaller or equal to pageSize. It is not necessary but is most
// Efficient for the sizes to be some fraction of pageSize. pageSize does not determine
// The size of the physical pages underneath the virtual memory (that's given by the
// XMemAttributes) but is how much additional memory the allocator should allocate
// Each time you run out of space.
//
// PreallocatePages specifies how many pages to initially allocate. Specifying zero will
// Preallocate two pages by default.
//
// This class is NOT thread safe. You should protect this with the appropriate sync
// Primitives or, even better, use one linear allocator per thread.
//
// Pages are freed once the GPU is done with them. As such, you need to specify when a
// Page is in use and when it is no longer in use. Use RetirePages to prompt the
// Allocator to check if pages are no longer being used by the GPU. Use InsertFences to
// Mark all used pages as in-use by the GPU, removing them from the available pages
// List. It is recommended you call RetirePages and InsertFences once a frame, usually
// Just before Present().
//
// Why is RetirePages decoupled from InsertFences? It's possible that you might want to
// Reclaim pages more frequently than locking used pages. For example, if you find the
// Allocator is burning through pages too quickly you can call RetirePages to reclaim
// Some that the GPU has finished with. However, this adds additional CPU overhead so it
// Is left to you to decide. In most cases this is sufficient:
//
// Allocator.RetirePages();
// Allocator.InsertFences( pContext, 0 );
// Present(...);
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// Http://go.microsoft.com/fwlink/?LinkID=615561
// --------------------------------------------------------------------------------------
#pragma once
#include <atomic>

namespace DirectX
{
    class LinearAllocatorPage
    {
    public:
        LinearAllocatorPage() noexcept;
        LinearAllocatorPage(LinearAllocatorPage &&) = delete;
        LinearAllocatorPage&_operator=(LinearAllocatorPage &&) = delete;
        LinearAllocatorPage(LinearAllocatorPage const&) = delete;
        LinearAllocatorPage&_operator=(LinearAllocatorPage const&) = delete;
        size_t Suballocate(
            in_in_ size_t in_size,
            in_in_ size_t in_alignment
            );
        void* BaseMemory() const noexcept
        {
            return _memory;
        }

        ID3D12Resource* UploadResource() const noexcept
        {
            return _upload_resource.Get();
        }

        D3D12_GPU_VIRTUAL_ADDRESS GpuAddress() const noexcept
        {
            return _gpu_address;
        }

        size_t BytesUsed() const noexcept
        {
            return _offset;
        }

        size_t Size() const noexcept
        {
            return _size;
        }

        void AddRef() noexcept
        {
            _ref_count.fetch_add(1);
        }

        int32_t RefCount() const noexcept
        {
            return _ref_count.load();
        }

        void Release() noexcept;

    protected:
        friend class LinearAllocator;


    protected:
        LinearAllocatorPage* _prev_page;
        LinearAllocatorPage* _next_page;
        void* _memory;
        uint64_t _pending_fence;
        D3D12_GPU_VIRTUAL_ADDRESS _gpu_address;
        size_t _offset;
        size_t _size;
        Microsoft::WRL::ComPtr < ID3D12Resource > _upload_resource;

    private:
        std::_atomic < _int32_t > _ref_count;
    };
    class LinearAllocator
    {
    public:
        // These values will be rounded up to the nearest 64k.
        // You can specify zero for incrementalSizeBytes to increment
        // By 1 page (64k).
        LinearAllocator(
            in_in_ ID3D12Device* in_device,
            in_in_ size_t in_page_size,
            in_in_ size_t in_preallocate_bytes = 0
            ) noexcept (false);
        LinearAllocator(LinearAllocator &&) = default;
        LinearAllocator&_operator=(LinearAllocator &&) = default;
        LinearAllocator(LinearAllocator const&) = delete;
        LinearAllocator&_operator=(LinearAllocator const&) = delete;
        ~LinearAllocator();
        LinearAllocatorPage* FindPageForAlloc(
            in_in_ size_t in_requested_size,
            in_in_ size_t in_alignment
            );
        // Call this at least once a frame to check if pages have become available.
        void RetirePendingPages() noexcept;
        // Call this after you submit your work to the driver.
        // (e.g. immediately before Present.)
        void FenceCommittedPages(in_in_ ID3D12CommandQueue* in_command_queue);
        // Throws away all currently unused pages
        void Shrink() noexcept;
        // Statistics
        size_t CommittedPageCount() const noexcept
        {
            return _num_pending;
        }

        size_t TotalPageCount() const noexcept
        {
            return _total_pages;
        }

        size_t CommittedMemoryUsage() const noexcept
        {
            return _num_pending* _increment;
        }

        size_t TotalMemoryUsage() const noexcept
        {
            return _total_pages* _increment;
        }

        size_t PageSize() const noexcept
        {
            return _increment;
        }


    private:
        LinearAllocatorPage* GetPageForAlloc(
            size_t in_size_bytes,
            size_t in_alignment
            );
        LinearAllocatorPage* GetCleanPageForAlloc();
        LinearAllocatorPage* FindPageForAlloc(
            LinearAllocatorPage* in_list,
            size_t in_size_bytes,
            size_t in_alignment
            ) noexcept;
        LinearAllocatorPage* GetNewPage();
        void UnlinkPage(LinearAllocatorPage* in_page) noexcept;
        void LinkPage(
            LinearAllocatorPage* in_page,
            LinearAllocatorPage*&in_list
            ) noexcept;
        void LinkPageChain(
            LinearAllocatorPage* in_page,
            LinearAllocatorPage*&in_list
            ) noexcept;
        void ReleasePage(LinearAllocatorPage* in_page) noexcept;
        void FreePages(LinearAllocatorPage* in_list) noexcept;

    private:
        LinearAllocatorPage* _pending_pages;
        // Pages in use by the GPU
        LinearAllocatorPage* _used_pages;
        // Pages to be submitted to the GPU
        LinearAllocatorPage* _unused_pages;
        // Pages not being used right now
        size_t _increment;
        size_t _num_pending;
        size_t _total_pages;
        uint64_t _fence_count;
        Microsoft::WRL::ComPtr < ID3D12Device > _device;
        Microsoft::WRL::ComPtr < ID3D12Fence > _fence;
#if defined (_DEBUG) || defined (PROFILE)
            // Debug info
            const wchar_t* GetDebugName() const noexcept
            {
                return _debug_name.c_str();
            }

            void SetDebugName(const wchar_t* in_name);
            void SetDebugName(const char* in_name);
#endif

#if defined (_DEBUG) || defined (PROFILE)
            std::_wstring _debug_name;
            static void ValidateList(LinearAllocatorPage* in_list);
            void ValidatePageLists();
            void SetPageDebugName(LinearAllocatorPage* in_list) noexcept;
#endif

    };
}
