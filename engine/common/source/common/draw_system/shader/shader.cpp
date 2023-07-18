#include "common/common_pch.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/i_resource.h"
#include "common/draw_system/shader/constant_buffer.h"
#include "common/draw_system/shader/constant_buffer_info.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/unordered_access_info.h"

std::shared_ptr < ConstantBuffer > MakeConstantBuffer(
    DrawSystem* const in_draw_system,
    const std::shared_ptr < ConstantBufferInfo >&in_constant_buffer_info
    )
{
    if (nullptr == in_constant_buffer_info)
    {
        return nullptr;
    }
    const size_t constant_buffer_size = in_constant_buffer_info->GetBufferSize();
    const void* const data = in_constant_buffer_info->GetBufferData();
    const D3D12_SHADER_VISIBILITY visiblity = in_constant_buffer_info->GetVisiblity();
    const int frame_count = in_draw_system->GetBackBufferCount();
    auto constant_buffer = std::make_shared < ConstantBuffer > (
        frame_count,
        constant_buffer_size,
        in_draw_system->MakeHeapWrapperCbvSrvUav(),
        data,
        visiblity
        );
    return constant_buffer;
}

static Microsoft::WRL::ComPtr < ID3D12RootSignature > MakeRootSignatureLocal(
    ID3D12Device* const in_device,
    const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC&in_root_signature_desc
    )
{
    Microsoft::WRL::ComPtr < ID3D12RootSignature > root_signature;

    {
        Microsoft::WRL::ComPtr < ID3DBlob > signature;
        Microsoft::WRL::ComPtr < ID3DBlob > error;
        DX::ThrowIfFailed(D3D12SerializeVersionedRootSignature(
            &in_root_signature_desc,
            &signature,
            &error
            ));
        DX::ThrowIfFailed(in_device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(root_signature.ReleaseAndGetAddressOf())
            ));
    }
    static int s_trace = 0;
    const std::wstring name = (std::wstring(L"RootSignature:") + std::to_wstring(s_trace++)) .c_str();
    root_signature->SetName(name.c_str());
    return root_signature;
}

static void RemoveDenyFlag(
    D3D12_ROOT_SIGNATURE_FLAGS&in_flag,
    const D3D12_SHADER_VISIBILITY in_visiblity
    )
{
    switch (in_visiblity)
    {
    default:
        break;
    case D3D12_SHADER_VISIBILITY_ALL:
        in_flag &= ~ (D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS | \
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | \
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | \
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | \
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS | \
            D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS | \
            D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS);
        break;
    case D3D12_SHADER_VISIBILITY_VERTEX:
        in_flag &= ~ (D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS);
        break;
    case D3D12_SHADER_VISIBILITY_HULL:
        in_flag &= ~ (D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
        break;
    case D3D12_SHADER_VISIBILITY_DOMAIN:
        in_flag &= ~ (D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);
        break;
    case D3D12_SHADER_VISIBILITY_GEOMETRY:
        in_flag &= ~ (D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
        break;
    case D3D12_SHADER_VISIBILITY_PIXEL:
        in_flag &= ~ (D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
        break;
    case D3D12_SHADER_VISIBILITY_AMPLIFICATION:
        in_flag &= ~ (D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS);
        break;
    case D3D12_SHADER_VISIBILITY_MESH:
        in_flag &= ~ (D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS);
        break;
    }

    return;
}

// Https://docs.microsoft.com/en-us/windows/win32/direct3d12/creating-a-root-signature
Microsoft::WRL::ComPtr < ID3D12RootSignature > MakeRootSignature(
    ID3D12Device* const in_device,
    const std::vector < std::shared_ptr < ShaderResourceInfo > >&in_shader_texture_info_array,
    const std::vector < std::shared_ptr < ConstantBuffer > >&in_constant_buffer_array,
    const std::vector < std::shared_ptr < UnorderedAccessInfo > >&in_array_unordered_access_info
    )
{
    Microsoft::WRL::ComPtr < ID3D12RootSignature > root_signature;
    in_shader_texture_info_array;
    in_constant_buffer_array;
    in_array_unordered_access_info;
    D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | \
        D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS | \
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | \
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | \
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | \
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS\
        ;
    std::vector < D3D12_ROOT_PARAMETER1 > root_paramter_array;
    std::vector < D3D12_STATIC_SAMPLER_DESC > static_sampler_desc_array;
    std::vector < std::shared_ptr < D3D12_DESCRIPTOR_RANGE1 >> descriptor_range_array;
    // B0,b1,b2,...
    if (0 < in_constant_buffer_array.size())
    {
        int trace = 0;
        for (const auto& iter : in_constant_buffer_array)
        {
            auto descriptor_range = std::make_shared < D3D12_DESCRIPTOR_RANGE1 >();
            descriptor_range_array.push_back(descriptor_range);
            descriptor_range->BaseShaderRegister = trace;
            trace += 1;
            descriptor_range->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
            descriptor_range->NumDescriptors = 1;
            descriptor_range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            descriptor_range->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            D3D12_ROOT_PARAMETER1 param;
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param.ShaderVisibility = iter->GetVisiblity();
            param.DescriptorTable.NumDescriptorRanges = 1;
            param.DescriptorTable.pDescriptorRanges = descriptor_range.get();
            root_paramter_array.push_back(param);
            RemoveDenyFlag(
                flags,
                param.ShaderVisibility
            );
        }
    }
    // U0,u1,u2,...
    if (0 < in_array_unordered_access_info.size())
    {
        int trace = 0;
        for (const auto&iter : in_array_unordered_access_info)
        {
            auto descriptor_range = std::make_shared < D3D12_DESCRIPTOR_RANGE1 > ();
            descriptor_range_array.push_back(descriptor_range);
            descriptor_range->BaseShaderRegister = trace;
            trace += 1;
            descriptor_range->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
            descriptor_range->NumDescriptors = 1;
            descriptor_range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            descriptor_range->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            D3D12_ROOT_PARAMETER1 param;
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param.ShaderVisibility = iter->GetVisiblity();
            param.DescriptorTable.NumDescriptorRanges = 1;
            param.DescriptorTable.pDescriptorRanges = descriptor_range.get();
            root_paramter_array.push_back(param);
            RemoveDenyFlag(
                flags,
                param.ShaderVisibility
                );
        }
    }
    // T0,t1,t2,...
    if (0 < in_shader_texture_info_array.size())
    {
        int trace = 0;
        for (const auto&iter : in_shader_texture_info_array)
        {
            auto descriptor_range = std::make_shared < D3D12_DESCRIPTOR_RANGE1 > ();
            descriptor_range_array.push_back(descriptor_range);
            descriptor_range->BaseShaderRegister = trace;
            trace += 1;
            descriptor_range->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
            descriptor_range->NumDescriptors = 1;
            descriptor_range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            descriptor_range->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            D3D12_ROOT_PARAMETER1 param;
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            param.ShaderVisibility = iter->GetVisiblity();
            param.DescriptorTable.NumDescriptorRanges = 1;
            param.DescriptorTable.pDescriptorRanges = descriptor_range.get();
            root_paramter_array.push_back(param);
            RemoveDenyFlag(
                flags,
                param.ShaderVisibility
                );
        }
    }
    // S0,s1,s2,...
    if (0 < in_shader_texture_info_array.size())
    {
#if 1
            for (const auto&iter : in_shader_texture_info_array)
            {
                if (false == iter->GetUseSampler())
                {
                    continue;
                }
                static_sampler_desc_array.push_back(iter->GetStaticSamplerDesc());
            }
#else
            int offset_table_start = 0;
            for (const auto&iter : in_shader_texture_info_array)
            {
                if (false == iter->GetUseSampler())
                {
                    continue;
                }
                auto descriptor_range = std::make_shared < D3D12_DESCRIPTOR_RANGE1 > ();
                descriptor_range_array.push_back(descriptor_range);
                descriptor_range->BaseShaderRegister = 0;
                descriptor_range->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
                descriptor_range->NumDescriptors = 1;
                descriptor_range->OffsetInDescriptorsFromTableStart = offset_table_start;
                offset_table_start += descriptor_range->NumDescriptors;
                descriptor_range->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                D3D12_ROOT_PARAMETER1 param;
                param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                param.ShaderVisibility = iter->GetVisiblity();
                param.DescriptorTable.NumDescriptorRanges = 1;
                param.DescriptorTable._descriptor_ranges = descriptor_range.get();
                root_paramter_array.push_back(param);
                RemoveDenyFlag(
                    flags,
                    param.ShaderVisibility
                    );
            }
#endif

    }
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc(
        (UINT) root_paramter_array.size(),
        root_paramter_array.size() ?&root_paramter_array[0] : nullptr,
        (UINT) static_sampler_desc_array.size(),
        static_sampler_desc_array.size() ?&static_sampler_desc_array[0] : nullptr,
        flags
        );
    root_signature = MakeRootSignatureLocal(
        in_device,
        root_signature_desc
        );
    return root_signature;
}

Microsoft::WRL::ComPtr < ID3D12PipelineState > MakePipelineStateComputeShader(
    ID3D12Device2* const in_device,
    const Microsoft::WRL::ComPtr < ID3D12RootSignature >&in_root_signature,
    const std::shared_ptr < std::vector < uint8_t >>&in_compute_shader_data
    )
{
    Microsoft::WRL::ComPtr < ID3D12PipelineState > pipeline_state;
    // Create the PSO for GenerateMips shader.
    struct PipelineStateStream
    {
    public:
        CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE in_root_signature;
        CD3DX12_PIPELINE_STATE_STREAM_CS CS;
    }pipeline_state_stream;
    pipeline_state_stream.in_root_signature = in_root_signature.Get();
    if (nullptr != in_compute_shader_data)
    {
        pipeline_state_stream.CS =
        {
            in_compute_shader_data->data(), in_compute_shader_data->size()};
    }
    D3D12_PIPELINE_STATE_STREAM_DESC pipeline_state_stream_desc =
    {
        sizeof (PipelineStateStream),&pipeline_state_stream};
    DX::ThrowIfFailed(in_device->CreatePipelineState(
        &pipeline_state_stream_desc,
        IID_PPV_ARGS(pipeline_state.ReleaseAndGetAddressOf())
        ));
    return pipeline_state;
}

Microsoft::WRL::ComPtr < ID3D12PipelineState > MakePipelineState(
    ID3D12Device* const in_device,
    const Microsoft::WRL::ComPtr < ID3D12RootSignature >& in_root_signature,
    const std::shared_ptr < std::vector < uint8_t >>& in_vertex_shader_data,
    const std::shared_ptr < std::vector < uint8_t >>& in_geometry_shader_data,
    const std::shared_ptr < std::vector < uint8_t >>& in_pixel_shader_data,
    const ShaderPipelineStateData& in_pipeline_state_data
    )
{
    Microsoft::WRL::ComPtr < ID3D12PipelineState > pipeline_state;
    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
    pso_desc.pRootSignature = in_root_signature.Get();
    if (nullptr != in_vertex_shader_data)
    {
        pso_desc.VS =
        {
            in_vertex_shader_data->data(), in_vertex_shader_data->size()
        };
    }
    if (nullptr != in_pixel_shader_data)
    {
        pso_desc.PS =
        {
            in_pixel_shader_data->data(), in_pixel_shader_data->size()
        };
    }
    // D3D12_SHADER_BYTECODE DS;
    // D3D12_SHADER_BYTECODE HS;
    if (nullptr != in_geometry_shader_data)
    {
        pso_desc.GS =
        {
            in_geometry_shader_data->data(), in_geometry_shader_data->size()
        };
    }
    // D3D12_STREAM_OUTPUT_DESC StreamOutput;
    pso_desc.BlendState = in_pipeline_state_data._blend_state;
    // CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.RasterizerState = in_pipeline_state_data._rasterizer_state;
    // CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso_desc.DepthStencilState = in_pipeline_state_data._depth_stencil_state;
    // PsoDesc.DepthStencilState.DepthEnable = FALSE;
    // PsoDesc.DepthStencilState.StencilEnable = FALSE;
    pso_desc.InputLayout =
    {
        &in_pipeline_state_data._input_element_desc_array[0], (UINT) in_pipeline_state_data._input_element_desc_array.\
            size()
    };
    // D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
    pso_desc.PrimitiveTopologyType = in_pipeline_state_data._primitive_topology_type;
    pso_desc.NumRenderTargets = (UINT) in_pipeline_state_data._render_target_format.size();
    // (UINT)numRenderTargets; //1
    for (int index = 0; index < (int) pso_desc.NumRenderTargets;++ index)
    {
        pso_desc.RTVFormats[index] = in_pipeline_state_data._render_target_format[index];
        // DXGI_FORMAT_B8G8R8A8_UNORM;
    }
    pso_desc.DSVFormat = in_pipeline_state_data._depth_stencil_view_format;
    // DXGI_FORMAT_UNKNOWN; // m_deviceResources->GetDepthBufferFormat();
    pso_desc.SampleDesc.Count = 1;
    // UINT NodeMask;
    // D3D12_CACHED_PIPELINE_STATE CachedPSO;
    // D3D12_PIPELINE_STATE_FLAGS Flags;
    DX::ThrowIfFailed(in_device->CreateGraphicsPipelineState(
        &pso_desc,
        IID_PPV_ARGS(pipeline_state.ReleaseAndGetAddressOf())
        ));
    pipeline_state->SetName(L"PipelineState");
    return pipeline_state;
}

Shader::Shader(
    DrawSystem* const in_draw_system,
    const ShaderPipelineStateData&in_pipeline_state_data,
    const std::shared_ptr < std::vector < uint8_t > >&in_vertex_shader_data,
    const std::shared_ptr < std::vector < uint8_t > >&in_geometry_shader_data,
    const std::shared_ptr < std::vector < uint8_t > >&in_pixel_shader_data,
    const std::vector < std::shared_ptr < ShaderResourceInfo > >&in_array_shader_resource_info,
    const std::vector < std::shared_ptr < ConstantBufferInfo > >&in_array_shader_constants_info,
    const std::shared_ptr < std::vector < uint8_t > >&in_compute_shader_data,
    const std::vector < std::shared_ptr < UnorderedAccessInfo > >&in_array_unordered_access_info
    ) 
    : IResource(in_draw_system)
    , _pipeline_state_data(in_pipeline_state_data)
    , _vertex_shader_data(in_vertex_shader_data)
    , _geometry_shader_data(in_geometry_shader_data)
    , _pixel_shader_data(in_pixel_shader_data)
    , _array_shader_resource_info(in_array_shader_resource_info)
    , _array_shader_constants_info(in_array_shader_constants_info)
    , _compute_shader_data(in_compute_shader_data)
    , _array_unordered_access_info(in_array_unordered_access_info)
{
    for (auto&iter : _array_shader_constants_info)
    {
        auto result = MakeConstantBuffer(
            in_draw_system,
            iter
            );
        if (result)
        {
            _array_constant_buffer.push_back(result);
        }
    }
    return;
}

Shader::~Shader()
{
    // Nop
}

void Shader::SetDebugName(const std::string& in_name)
{
    _debug_name = in_name;
}

void Shader::SetActivate(
    ID3D12GraphicsCommandList* const in_command_list,
    const int in_frame_index
    )
{
    if (false == _pipeline_state_data._compute_shader)
    {
        in_command_list->SetGraphicsRootSignature(_root_signature.Get());
    }
    else
    {
        in_command_list->SetComputeRootSignature(_root_signature.Get());
    }
    in_command_list->SetPipelineState(_pipeline_state.Get());
    int root_paramter_index = 0;
    // B0,b1,b2,...
    for (const auto& iter : _array_constant_buffer)
    {
        iter->Activate(
            in_command_list,
            root_paramter_index,
            in_frame_index
            );
        root_paramter_index += 1;
    }
    // U0,u1,u2,...
    for (const auto& iter : _array_unordered_access_info)
    {
        iter->Activate(
            in_command_list,
            root_paramter_index
            );
        root_paramter_index += 1;
    }
    // T0,t1,t2,...
    for (const auto& iter : _array_shader_resource_info)
    {
        iter->Activate(
            in_command_list,
            root_paramter_index
            );
        root_paramter_index += 1;
    }
    // S0,s1,s2,...
    // For(const auto& iter : m_arrayShaderResourceInfo)
    // {
    // If (false == iter->GetUseSampler())
    // {
    // Continue;
    // }
    // Iter->ActivateSampler(pCommandList, rootParamterIndex);
    // RootParamterIndex += 1;
    // }
}

void Shader::SetShaderResourceViewHandle(
    const int in_index,
    const std::shared_ptr < HeapWrapperItem >& in_shader_resource_view_handle
    )
{
    if ((0 <= in_index) && (in_index < (int) _array_shader_resource_info.size()))
    {
        _array_shader_resource_info[in_index]->SetShaderResourceViewHandle(in_shader_resource_view_handle);
    }
    return;
}

void Shader::SetUnorderedAccessViewHandle(
    const int in_index,
    const std::shared_ptr < HeapWrapperItem >&in_unordered_access_view_handle
    )
{
    if ((0 <= in_index) && (in_index < (int) _array_unordered_access_info.size()))
    {
        _array_unordered_access_info[in_index]->SetUnorderedAccessViewHandle(in_unordered_access_view_handle);
    }
    return;
}

void Shader::SetConstantBufferData(
    const int in_index,
    const std::vector < float >&in_data
    )
{
    if ((0 <= in_index) && (in_index < (int) _array_shader_constants_info.size()))
    {
        auto&shader_constant_info =* _array_shader_constants_info[in_index];
        const void* const data = &in_data[0];
        shader_constant_info.UpdateData(
            data,
            sizeof (float) * in_data.size()
            );
    }
    return;
}

void Shader::OnDeviceLost()
{
    _root_signature.Reset();
    _pipeline_state.Reset();
    for (auto&constant_buffer : _array_constant_buffer)
    {
        constant_buffer->DeviceLost();
    }
}

void Shader::OnDeviceRestored(
    ID3D12GraphicsCommandList* const,
    ID3D12Device2* const in_device
    )
{
    for (auto&constant_buffer : _array_constant_buffer)
    {
        constant_buffer->DeviceRestored(in_device);
    }
    _root_signature = MakeRootSignature(
        in_device,
        _array_shader_resource_info,
        _array_constant_buffer,
        _array_unordered_access_info
        );
    if (true == _pipeline_state_data._compute_shader)
    {
        _pipeline_state = MakePipelineStateComputeShader(
            in_device,
            _root_signature,
            _compute_shader_data
            );
    }
    else
    {
        _pipeline_state = MakePipelineState(
            in_device,
            _root_signature,
            _vertex_shader_data,
            _geometry_shader_data,
            _pixel_shader_data,
            _pipeline_state_data
            );
    }
    return;
}

