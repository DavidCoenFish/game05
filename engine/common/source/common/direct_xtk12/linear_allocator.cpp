// --------------------------------------------------------------------------------------
// File: LinearAllocator.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// Http://go.microsoft.com/fwlink/?LinkID=615561
// --------------------------------------------------------------------------------------
#include "common/common_pch.h"

#include "common/direct_xtk12/direct_x_helpers.h"
#include "common/direct_xtk12/linear_allocator.h"
#include "common/draw_system/d3dx12.h"
#include "common/log/log.h"

// Set this to 1 to enable some additional debug validation
#define VALIDATE_LISTS 0
#if VALIDATE_LISTS
    #
    include < unordered_set > #endif

using namespace DirectX;
using Microsoft::WRL::ComPtr;
LinearAllocatorPage::LinearAllocatorPage() noexcept 
    : prev_page(nullptr)
    , next_page(nullptr)
    , _memory(nullptr)
    , _pending_fence(0)
    , _gpu_address{}
    , _offset(0)
    , _size(0)
    , _ref_count(1){}

size_t LinearAllocatorPage::Suballocate(
    in_in_ size_t in_size,
    in_in_ size_t in_alignment
    )
{
    size_t offset = AlignUp(
        _offset,
        in_alignment
        );
    if (offset + in_size > _size)
    {
        // Use of suballocate should be limited to pages with free space,
        // So really shouldn't happen.
        throw std::exception("LinearAllocatorPage::Suballocate");
    }
    _offset = offset + in_size;
    return offset;
}

void LinearAllocatorPage::Release() noexcept
{
    assert(_ref_count > 0);
    if (_ref_count.fetch_sub(1) == 1)
    {
        _upload_resource->Unmap(
            0,
            nullptr
            );
        delete this;
    }
}

// --------------------------------------------------------------------------------------
LinearAllocator::LinearAllocator(
    in_in_ ID3D12Device* in_device,
    in_in_ size_t in_page_size,
    in_in_ size_t in_preallocate_bytes
    ) noexcept (false) 
    : _pending_pages(nullptr)
    , _used_pages(nullptr)
    , _unused_pages(nullptr)
    , _increment(in_page_size)
    , _num_pending(0)
    , _total_pages(0)
    , _fence_count(0)
    , _device(in_device)
{
    assert(in_device != nullptr);
#if defined (_DEBUG) || defined (PROFILE)
        _debug_name = L"LinearAllocator";
#endif

    size_t preallocate_page_count = ((in_preallocate_bytes + in_page_size - 1) / in_page_size);
    for (size_t preallocate_pages = 0; in_preallocate_bytes != 0 && preallocate_pages < preallocate_page_count;++ 
        preallocate_pages)
    {
        if (GetNewPage() == nullptr)
        {
            LOG_MESSAGE_WARNING(
                "LinearAllocator failed to preallocate pages (%zu required bytes, %zu pages)\n",
                preallocate_page_count* _increment,
                preallocate_page_count
                );
            throw std::bad_alloc();
        }
    }
    DX::ThrowIfFailed(in_device->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_GRAPHICS_PPV_ARGS(_fence.ReleaseAndGetAddressOf())
        ));
}

LinearAllocator::~LinearAllocator()
{
    // Must wait for all pending fences!
    while (_pending_pages != nullptr)
    {
        RetirePendingPages();
    }
    assert(_pending_pages == nullptr);
    // Return all the memory
    FreePages(_unused_pages);
    FreePages(_used_pages);
    _pending_pages = nullptr;
    _used_pages = nullptr;
    _unused_pages = nullptr;
    _increment = 0;
}

LinearAllocatorPage* LinearAllocator::FindPageForAlloc(
    in_in_ size_t in_size,
    in_in_ size_t in_alignment
    )
{
#ifdef _DEBUG
        if (in_size > _increment) throw std::out_of_range("Size must be less or equal to the allocator's increment");
        if (in_alignment > _increment) throw std::out_of_range(
            "Alignment must be less or equal to the allocator's increment");
        if (in_size == 0) throw std::exception("Cannot honor zero size allocation request.");
#endif

    auto page = GetPageForAlloc(
        in_size,
        in_alignment
        );
    if (!page)
    {
        return nullptr;
    }
    return page;
}

// Call this after you submit your work to the driver.
void LinearAllocator::FenceCommittedPages(in_in_ ID3D12CommandQueue* in_command_queue)
{
    // No pending pages
    if (_used_pages == nullptr) return;
    // For all the used pages, fence them
    UINT num_ready = 0;
    LinearAllocatorPage* ready_pages = nullptr;
    LinearAllocatorPage* unready_pages = nullptr;
    LinearAllocatorPage* next_page = nullptr;
    for (auto page = _used_pages; page != nullptr; page = next_page)
    {
        next_page = page->_next_page;
        // Disconnect from the list
        page->_prev_page = nullptr;

            // This implies the allocator is the only remaining reference to the page, and therefore the memory is ready for re-use.
        if (page->RefCount() == 1)
        {
            // Signal the fence
            num_ready++;
            page->_pending_fence =++ _fence_count;
            DX::ThrowIfFailed(in_command_queue->Signal(
                _fence.Get(),
                _fence_count
                ));
            // Link to the ready pages list
            page->_next_page = ready_pages;
            if (ready_pages) ready_pages->_prev_page = page;
            ready_pages = page;
        }
        else
        {
            // Link to the unready list
            page->_next_page = unready_pages;
            if (unready_pages) unready_pages->_prev_page = page;
            unready_pages = page;
        }
    }
    // Replace the used pages list with the new unready list
    _used_pages = unready_pages;
    // Append all those pages from the ready list to the pending list
    if (num_ready > 0)
    {
        _num_pending += num_ready;
        LinkPageChain(
            ready_pages,
            _pending_pages
            );
    }
#if VALIDATE_LISTS
        ValidatePageLists();
#endif

}

// Call this once a frame after all of your driver submissions.
// (immediately before or after Present-time)
void LinearAllocator::RetirePendingPages() noexcept
{
    uint64_t fence_value = _fence->GetCompletedValue();
    // For each page that we know has a fence pending, check it. If the fence has passed,
    // We can mark the page for re-use.
    auto page = _pending_pages;
    while (page != nullptr)
    {
        auto next_page = page->_next_page;
        assert(page->_pending_fence != 0);
        if (fence_value >= page->_pending_fence)
        {
            // Fence has passed. It is safe to use this page again.
            ReleasePage(page);
        }
        page = next_page;
    }
}

void LinearAllocator::Shrink() noexcept
{
    FreePages(_unused_pages);
    _unused_pages = nullptr;
#if VALIDATE_LISTS
        ValidatePageLists();
#endif

}

LinearAllocatorPage* LinearAllocator::GetCleanPageForAlloc()
{
    // Grab the first unused page, if one exists. Else, allocate a new page.
    auto page = _unused_pages;
    if (!page)
    {
        // Allocate a new page
        page = GetNewPage();
        if (!page)
        {
            return nullptr;
        }
    }
    // Mark this page as used
    UnlinkPage(page);
    LinkPage(
        page,
        _used_pages
        );
    assert(page->_offset == 0);
    return page;
}

LinearAllocatorPage* LinearAllocator::GetPageForAlloc(
    size_t in_size_bytes,
    size_t in_alignment
    )
{
    // Fast path
    if (in_size_bytes == _increment && (in_alignment == 0 || in_alignment == _increment))
    {
        return GetCleanPageForAlloc();
    }
    // Find a page in the pending pages list that has space.
    auto page = FindPageForAlloc(
        _used_pages,
        in_size_bytes,
        in_alignment
        );
    if (!page)
    {
        page = GetCleanPageForAlloc();
    }
    return page;
}

LinearAllocatorPage* LinearAllocator::FindPageForAlloc(
    LinearAllocatorPage* in_list,
    size_t in_size_bytes,
    size_t in_alignment
    ) noexcept
{
    for (auto page = in_list; page != nullptr; page = page->_next_page)
    {
        size_t offset = AlignUp(
            page->_offset,
            in_alignment
            );
        if (offset + in_size_bytes <= _increment) return page;
    }
    return nullptr;
}

LinearAllocatorPage* LinearAllocator::GetNewPage()
{
    CD3DX12_HEAP_PROPERTIES upload_heap_properties(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(_increment);
    // Allocate the upload heap
    ComPtr < ID3D12Resource > sp_resource;
    HRESULT hr = _device->CreateCommittedResource(
        &upload_heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_GRAPHICS_PPV_ARGS(sp_resource.ReleaseAndGetAddressOf())
        );
    if (FAILED(hr))
    {
        if (hr != E_OUTOFMEMORY)
        {
            LOG_MESSAGE_WARNING(
                "LinearAllocator::GetNewPage resource allocation failed due to unexpected error %08X\n",
                static_cast < unsigned int > (hr)
                );
        }
        return nullptr;
    }
#if defined (_DEBUG) || defined (PROFILE)
        sp_resource->SetName(_debug_name.in_empty() ? L"LinearAllocator" : _debug_name.c_str());
#endif

    // Get a pointer to the memory
    void* memory = nullptr;
    DX::ThrowIfFailed(sp_resource->Map(
        0,
        nullptr,
        &memory
        ));
    memset(
        memory,
        0,
        _increment
        );
    // Add the page to the page list
    auto page = new LinearAllocatorPage;
    page->_memory = memory;
    page->_gpu_address = sp_resource->GetGPUVirtualAddress();
    page->_size = _increment;
    page->_upload_resource.Swap(sp_resource);
    // Set as head of the list
    page->_next_page = _unused_pages;
    if (_unused_pages) _unused_pages->_prev_page = page;
    _unused_pages = page;
    _total_pages++;
#if VALIDATE_LISTS
        ValidatePageLists();
#endif

    return page;
}

void LinearAllocator::UnlinkPage(LinearAllocatorPage* in_page) noexcept
{
    if (in_page->_prev_page) in_page->_prev_page->_next_page = in_page->_next_page;
    // Check that it isn't the head of any of our tracked lists
    else if (in_page == _unused_pages) _unused_pages = in_page->_next_page;
    else if (in_page == _used_pages) _used_pages = in_page->_next_page;
    else if (in_page == _pending_pages) _pending_pages = in_page->_next_page;
    if (in_page->_next_page) in_page->_next_page->_prev_page = in_page->_prev_page;
    in_page->_next_page = nullptr;
    in_page->_prev_page = nullptr;
#if VALIDATE_LISTS
        ValidatePageLists();
#endif

}

void LinearAllocator::LinkPageChain(
    LinearAllocatorPage* in_page,
    LinearAllocatorPage*&in_list
    ) noexcept
{
#if VALIDATE_LISTS
        // Walk the chain and ensure it's not in the list twice
        for (LinearAllocatorPage* cur = in_list; cur != nullptr; cur = cur->_next_page)
        {
            assert(cur != in_page);
        }
#endif

    assert(in_page->_prev_page == nullptr);
    assert(in_list == nullptr || in_list->_prev_page == nullptr);
    // Follow chain to the end and append
    LinearAllocatorPage* last_page = nullptr;
    for (last_page = in_page; last_page->_next_page != nullptr; last_page = last_page->_next_page) {} last_page->
        _next_page = in_list;
    if (in_list) in_list->_prev_page = last_page;
    in_list = in_page;
#if VALIDATE_LISTS
        ValidatePageLists();
#endif

}

void LinearAllocator::LinkPage(
    LinearAllocatorPage* in_page,
    LinearAllocatorPage*&in_list
    ) noexcept
{
#if VALIDATE_LISTS
        // Walk the chain and ensure it's not in the list twice
        for (LinearAllocatorPage* cur = in_list; cur != nullptr; cur = cur->_next_page)
        {
            assert(cur != in_page);
        }
#endif

    assert(in_page->_next_page == nullptr);
    assert(in_page->_prev_page == nullptr);
    assert(in_list == nullptr || in_list->_prev_page == nullptr);
    in_page->_next_page = in_list;
    if (in_list) in_list->_prev_page = in_page;
    in_list = in_page;
#if VALIDATE_LISTS
        ValidatePageLists();
#endif

}

void LinearAllocator::ReleasePage(LinearAllocatorPage* in_page) noexcept
{
    assert(_num_pending > 0);
    _num_pending --;
    UnlinkPage(in_page);
    LinkPage(
        in_page,
        _unused_pages
        );
    // Reset the page offset (effectively erasing the memory)
    in_page->_offset = 0;
#ifdef _DEBUG
        memset(
            in_page->_memory,
            0,
            _increment
            );
#endif

#if VALIDATE_LISTS
        ValidatePageLists();
#endif

}

void LinearAllocator::FreePages(LinearAllocatorPage* in_page) noexcept
{
    while (in_page != nullptr)
    {
        auto next_page = in_page->_next_page;
        in_page->Release();
        in_page = next_page;
        assert(_total_pages > 0);
        _total_pages --;
    }
}

#if VALIDATE_LISTS
    void LinearAllocator::ValidateList(LinearAllocatorPage* in_list)
    {
        for (
            auto page = in_list,
            * last_page = nullptr; page != nullptr; last_page = page,
            page = page->_next_page
            )
        {
            if (page->_prev_page != last_page)
            {
                throw std::exception("Broken link to previous");
            }
        }
    }

    void LinearAllocator::ValidatePageLists()
    {
        ValidateList(_pending_pages);
        ValidateList(_used_pages);
        ValidateList(_unused_pages);
    }

#endif

#if defined (_DEBUG) || defined (PROFILE)
    void LinearAllocator::SetDebugName(const char* in_name)
    {
        wchar_t wname[MAX_PATH] = {};
        int result = MultiByteToWideChar(
            CP_UTF8,
            0,
            in_name,
            static_cast < int > (strlen(in_name)),
            wname,
            MAX_PATH
            );
        if (result > 0)
        {
            SetDebugName(wname);
        }
    }

    void LinearAllocator::SetDebugName(const wchar_t* in_name)
    {
        _debug_name = in_name;
        // Rename existing pages
        _fence->SetName(in_name);
        SetPageDebugName(_pending_pages);
        SetPageDebugName(_used_pages);
        SetPageDebugName(_unused_pages);
    }

    void LinearAllocator::SetPageDebugName(LinearAllocatorPage* in_list) noexcept
    {
        for (auto page = in_list; page != nullptr; page = page->_next_page)
        {
            page->_upload_resource->SetName(_debug_name.c_str());
        }
    }

#endif

