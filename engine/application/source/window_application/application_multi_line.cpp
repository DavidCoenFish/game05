#include "application_pch.h"

#include "window_application/application_multi_line.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/draw_system/shader/shader_resource.h"
#include "common/draw_system/shader/shader_resource_info.h"
#include "common/draw_system/shader/unordered_access.h"
#include "common/draw_system/shader/unordered_access_info.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/angle.h"
#include "common/math/matrix_float33.h"
#include "common/math/quaternion_float.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float3.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

struct ConstantBufferB0
{
    VectorFloat3 _camera_pos;
    float _camera_fov_horizontal;
    VectorFloat3 _camera_at;
    float _camera_fov_vertical;
    VectorFloat3 _camera_up;
    float _camera_far;
    float _camera_unit_pixel_size; //sin(radian_per_pixel)
    float _camera_radian_per_pixel;
    float _pad0[2];
};

struct ConstantBufferBackgroundB1
{
    VectorFloat2 _sun_azimuth_altitude;
    VectorFloat2 _sun_range;
    VectorFloat3 _sun_tint;
    float _sky_spread;
    VectorFloat3 _sky_tint;
    float _sky_turbitity;
    VectorFloat3 _ground_tint;
    float _pad0;
    VectorFloat3 _fog_tint;
    float _pad1;
};

static VectorFloat3 s_camera_pos(-1.0f, 0.0f, 0.0f);
static VectorFloat3 s_camera_at(1.0f, 0.0f, 0.0f);
static VectorFloat3 s_camera_up(0.0f, 1.0f, 0.0f);

IWindowApplication* const ApplicationMultiLine::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationMultiLine(
        in_hwnd,
        in_application_param
        );
}

ApplicationMultiLine::ApplicationMultiLine(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
    , _fov_vertical(Angle::DegToRadian(145.0f))
    , _fov_horizontal_calculated(0.0f)
    , _radian_per_pixel(0.0f)
    , _unit_pixel_size(0.0f)
    , _camera_pos(s_camera_pos)
    , _camera_at(s_camera_at)
    , _camera_up(s_camera_up)
    , _camera_far(1000.0f)
    , _input_q(false)
    , _input_w(false)
    , _input_e(false)
    , _input_a(false)
    , _input_s(false)
    , _input_d(false)
    , _input_u(false)
    , _input_i(false)
    , _input_o(false)
    , _input_j(false)
    , _input_k(false)
    , _input_l(false)
{
    LOG_MESSAGE(
        "ApplicationMultiLine  ctor %p",
        this
        );
    _draw_system = std::make_unique < DrawSystem > (in_hwnd);

    {
        _draw_resources = std::make_unique<DrawResources>();
        auto command_list = _draw_system->CreateCustomCommandList();

#if 1
        // Background
        {
            std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
            input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
                {
                    "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
                });

            auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "vertex_shader.cso");

            // Shader background
            {
                auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "pixel_shader_background.cso");
                std::vector < DXGI_FORMAT > render_target_format;
                render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
                ShaderPipelineStateData shader_pipeline_state_data(
                    input_element_desc_array,
                    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                    DXGI_FORMAT_UNKNOWN,
                    // DXGI_FORMAT_D32_FLOAT,
                    render_target_format,
                    CD3DX12_BLEND_DESC(D3D12_DEFAULT),
                    CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
                    CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
                );

                std::vector < std::shared_ptr < ShaderResourceInfo > > array_shader_resource_info;
                std::vector < std::shared_ptr < ConstantBufferInfo > > array_shader_constants_info;

                array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
                    ConstantBufferB0(),
                    D3D12_SHADER_VISIBILITY_PIXEL
                ));
                array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
                    ConstantBufferBackgroundB1(),
                    D3D12_SHADER_VISIBILITY_PIXEL
                ));

                _draw_resources->_background_shader = _draw_system->MakeShader(
                    command_list->GetCommandList(),
                    shader_pipeline_state_data,
                    vertex_shader_data,
                    nullptr,
                    pixel_shader_data,
                    array_shader_resource_info,
                    array_shader_constants_info
                );
                _draw_resources->_background_shader_constant_buffer = _draw_resources->_background_shader->MakeShaderConstantBuffer(_draw_system.get());
            }

            // Shader background grid
            {
                auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "pixel_shader_grid.cso");
                std::vector < DXGI_FORMAT > render_target_format;
                render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
                const auto blend_desc = ShaderPipelineStateData::FactoryBlendDescAlphaPremultiplied();

                ShaderPipelineStateData shader_pipeline_state_data(
                    input_element_desc_array,
                    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                    DXGI_FORMAT_UNKNOWN,
                    render_target_format,
                    blend_desc,
                    CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
                    CD3DX12_DEPTH_STENCIL_DESC()
                    );

                std::vector < std::shared_ptr < ShaderResourceInfo > > array_shader_resource_info;
                std::vector < std::shared_ptr < ConstantBufferInfo > > array_shader_constants_info;

                array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
                    ConstantBufferB0(),
                    D3D12_SHADER_VISIBILITY_PIXEL
                    ));

                _draw_resources->_background_shader_grid = _draw_system->MakeShader(
                    command_list->GetCommandList(),
                    shader_pipeline_state_data,
                    vertex_shader_data,
                    nullptr,
                    pixel_shader_data,
                    array_shader_resource_info,
                    array_shader_constants_info
                    );
                _draw_resources->_background_shader_grid_constant_buffer = _draw_resources->_background_shader_grid->MakeShaderConstantBuffer(_draw_system.get());
            }

            // Geometry Screen Quad
            {
                const auto vertex_data = VectorHelper::FactoryArrayLiteral(
                    {
                        -1.0f, -1.0f,
                        -1.0f, 1.0f,
                        1.0f, -1.0f,
                        1.0f, 1.0f,
                    }
                );
                _draw_resources->_background_geometry = _draw_system->MakeGeometryGeneric(
                    command_list->GetCommandList(),
                    D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
                    input_element_desc_array,
                    vertex_data,
                    2
                    );
            }
        }
#endif

        // std::shared_ptr<Shader> _multi_line_compute;
        {
        }
#if defined(DSC_UNORDED_ACCESS)
        // std::shared_ptr<UnorderedAccess> _multi_line_data_pos_thick;
        {
            std::vector< RenderTargetFormatData > targetFormatDataArray;
            targetFormatDataArray.push_back(RenderTargetFormatData(DXGI_FORMAT_R32G32B32A32_FLOAT));

            const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                4,
                4,
                1,
                1,
                1,
                0,
                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
            );

            D3D12_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc({});
            unordered_access_view_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            unordered_access_view_desc.Texture2D.MipSlice = 0;
            unordered_access_view_desc.Texture2D.PlaneSlice = 0;

            const float thickness = 0.5f;
            const VectorFloat4 data_literal[] = {
                VectorFloat4(-0.5f, 0.0f, -0.5f, thickness),
                VectorFloat4(-0.5f, 0.0f, -0.5f, thickness),
                VectorFloat4(-0.5f, 0.0f, 0.5f, thickness),
                VectorFloat4(-0.5f, 0.0f, 0.5f, thickness),
                VectorFloat4(0.5f, 0.0f, -0.5f, thickness),
                VectorFloat4(0.5f, 0.0f, -0.5f, thickness),
                VectorFloat4(0.5f, 0.0f, 0.5f, thickness),
                VectorFloat4(0.5f, 0.0f, 0.5f, thickness),
                VectorFloat4(-0.5f, 1.0f, -0.5f, thickness),
                VectorFloat4(-0.5f, 1.0f, 0.5f, thickness),
                VectorFloat4(0.5f, 1.0f, -0.5f, thickness),
                VectorFloat4(0.5f, 1.0f, 0.5f, thickness),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4()
            };
            auto data = VectorHelper::FactoryArrayLiteral(data_literal);

            _draw_resources->_multi_line_data_pos_thick = _draw_system->MakeUnorderedAccess(
                command_list->GetCommandList(),
                desc,
                unordered_access_view_desc,
                true,
                data
                );
        }

        // std::shared_ptr<UnorderedAccess> _multi_line_data_dir_length;
        {
            std::vector< RenderTargetFormatData > targetFormatDataArray;
            targetFormatDataArray.push_back(RenderTargetFormatData(DXGI_FORMAT_R32G32B32A32_FLOAT));

            const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                4,
                4,
                1,
                1,
                1,
                0,
                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
            );

            D3D12_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc({});
            unordered_access_view_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            unordered_access_view_desc.Texture2D.MipSlice = 0;
            unordered_access_view_desc.Texture2D.PlaneSlice = 0;

            const float length = 1.0f;
            const VectorFloat4 data_literal[] = {
                VectorFloat4(0.0f, 1.0f, 0.0f, length),
                VectorFloat4(0.0f, 0.0f, 1.0f, length),
                VectorFloat4(0.0f, 1.0f, 0.0f, length),
                VectorFloat4(1.0f, 0.0f, 0.0f, length),
                VectorFloat4(0.0f, 1.0f, 0.0f, length),
                VectorFloat4(-1.0f, 0.0f, 0.0f, length),
                VectorFloat4(0.0f, 1.0f, 0.0f, length),
                VectorFloat4(0.0f, 0.0f, -1.0f, length),
                VectorFloat4(0.0f, 0.0f, 1.0f, length),
                VectorFloat4(1.0f, 0.0f, 0.0f, length),
                VectorFloat4(-1.0f, 0.0f, 0.0f, length),
                VectorFloat4(0.0f, 0.0f, -1.0f, length),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4()
                };
            auto data = VectorHelper::FactoryArrayLiteral(data_literal);

            _draw_resources->_multi_line_data_dir_length = _draw_system->MakeUnorderedAccess(
                command_list->GetCommandList(),
                desc,
                unordered_access_view_desc,
                true,
                data
            );
        }

        // std::shared_ptr<UnorderedAccess> _multi_line_data_colour;
        {
            std::vector< RenderTargetFormatData > targetFormatDataArray;
            targetFormatDataArray.push_back(RenderTargetFormatData(DXGI_FORMAT_R32G32B32A32_FLOAT));

            const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                4,
                4,
                1,
                1,
                1,
                0,
                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
            );

            D3D12_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc({});
            unordered_access_view_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            unordered_access_view_desc.Texture2D.MipSlice = 0;
            unordered_access_view_desc.Texture2D.PlaneSlice = 0;

            const VectorFloat4 data_literal[] = {
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4()
            };
            auto data = VectorHelper::FactoryArrayLiteral(data_literal);

            _draw_resources->_multi_line_data_colour = _draw_system->MakeUnorderedAccess(
                command_list->GetCommandList(),
                desc,
                unordered_access_view_desc,
                true,
                data
            );
        }
#else
        // std::shared_ptr<ShaderResource> _multi_line_data_pos_thick;
        {
            D3D12_RESOURCE_DESC desc = {
                D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
                D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
                4, //UINT64 Width;
                4, //UINT Height;
                1, //UINT16 DepthOrArraySize;
                1, //UINT16 MipLevels;
                DXGI_FORMAT_R32G32B32A32_FLOAT, //DXGI_FORMAT Format;
                DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
                D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
                D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE //D3D12_RESOURCE_FLAGS Flags;
            };
            // Describe and create a SRV for the texture.
            D3D12_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
            shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            shader_resource_view_desc.Format = desc.Format;
            shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            shader_resource_view_desc.Texture2D.MipLevels = 1;

            const float thickness = 1.0f;
            const VectorFloat4 data_literal[] = {
                VectorFloat4(-0.5f, 0.0f, -0.5f, thickness),
                VectorFloat4(-0.5f, 0.0f, -0.5f, thickness),
                VectorFloat4(-0.5f, 0.0f, 0.5f, thickness),
                VectorFloat4(-0.5f, 0.0f, 0.5f, thickness),
                VectorFloat4(0.5f, 0.0f, -0.5f, thickness),
                VectorFloat4(0.5f, 0.0f, -0.5f, thickness),
                VectorFloat4(0.5f, 0.0f, 0.5f, thickness),
                VectorFloat4(0.5f, 0.0f, 0.5f, thickness),
                VectorFloat4(-0.5f, 1.0f, -0.5f, thickness),
                VectorFloat4(-0.5f, 1.0f, 0.5f, thickness),
                VectorFloat4(0.5f, 1.0f, -0.5f, thickness),
                VectorFloat4(0.5f, 1.0f, 0.5f, thickness),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4()
            };
            auto data = VectorHelper::FactoryArrayLiteral(data_literal);

            _draw_resources->_multi_line_data_pos_thick = _draw_system->MakeShaderResource(
                command_list->GetCommandList(), //ID3D12GraphicsCommandList* const in_command_list,
                _draw_system->MakeHeapWrapperCbvSrvUav(), //const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
                desc, //const D3D12_RESOURCE_DESC & in_desc,
                shader_resource_view_desc, //const D3D12_SHADER_RESOURCE_VIEW_DESC & in_shader_resource_view_desc,
                data //const std::vector < uint8_t >&in_data
                );
        }

        // std::shared_ptr<ShaderResource> _multi_line_data_dir_length;
        {
            D3D12_RESOURCE_DESC desc = {
                D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
                D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
                4, //UINT64 Width;
                4, //UINT Height;
                1, //UINT16 DepthOrArraySize;
                1, //UINT16 MipLevels;
                DXGI_FORMAT_R32G32B32A32_FLOAT, //DXGI_FORMAT Format;
                DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
                D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
                D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE //D3D12_RESOURCE_FLAGS Flags;
            };
            // Describe and create a SRV for the texture.
            D3D12_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
            shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            shader_resource_view_desc.Format = desc.Format;
            shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            shader_resource_view_desc.Texture2D.MipLevels = 1;

            const float length = 1.0f;
            const VectorFloat4 data_literal[] = {
                VectorFloat4(0.0f, 1.0f, 0.0f, length),
                VectorFloat4(0.0f, 0.0f, 1.0f, length),
                VectorFloat4(0.0f, 1.0f, 0.0f, length),
                VectorFloat4(1.0f, 0.0f, 0.0f, length),
                VectorFloat4(0.0f, 1.0f, 0.0f, length),
                VectorFloat4(-1.0f, 0.0f, 0.0f, length),
                VectorFloat4(0.0f, 1.0f, 0.0f, length),
                VectorFloat4(0.0f, 0.0f, -1.0f, length),
                VectorFloat4(0.0f, 0.0f, 1.0f, length),
                VectorFloat4(1.0f, 0.0f, 0.0f, length),
                VectorFloat4(-1.0f, 0.0f, 0.0f, length),
                VectorFloat4(0.0f, 0.0f, -1.0f, length),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4()
            };
            auto data = VectorHelper::FactoryArrayLiteral(data_literal);

            _draw_resources->_multi_line_data_dir_length = _draw_system->MakeShaderResource(
                command_list->GetCommandList(), //ID3D12GraphicsCommandList* const in_command_list,
                _draw_system->MakeHeapWrapperCbvSrvUav(), //const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
                desc, //const D3D12_RESOURCE_DESC & in_desc,
                shader_resource_view_desc, //const D3D12_SHADER_RESOURCE_VIEW_DESC & in_shader_resource_view_desc,
                data //const std::vector < uint8_t >&in_data
            );
        }

        // std::shared_ptr<ShaderResource> _multi_line_data_colour;
        {
            D3D12_RESOURCE_DESC desc = {
                D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
                D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
                4, //UINT64 Width;
                4, //UINT Height;
                1, //UINT16 DepthOrArraySize;
                1, //UINT16 MipLevels;
                DXGI_FORMAT_R32G32B32A32_FLOAT, //DXGI_FORMAT Format;
                DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
                D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
                D3D12_RESOURCE_FLAG_NONE //D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE //D3D12_RESOURCE_FLAGS Flags;
            };
            // Describe and create a SRV for the texture.
            D3D12_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
            shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            shader_resource_view_desc.Format = desc.Format;
            shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            shader_resource_view_desc.Texture2D.MipLevels = 1;

            const VectorFloat4 data_literal[] = {
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4(),
                VectorFloat4()
            };
            auto data = VectorHelper::FactoryArrayLiteral(data_literal);

            _draw_resources->_multi_line_data_colour = _draw_system->MakeShaderResource(
                command_list->GetCommandList(), //ID3D12GraphicsCommandList* const in_command_list,
                _draw_system->MakeHeapWrapperCbvSrvUav(), //const std::shared_ptr < HeapWrapperItem >&in_shader_resource,
                desc, //const D3D12_RESOURCE_DESC & in_desc,
                shader_resource_view_desc, //const D3D12_SHADER_RESOURCE_VIEW_DESC & in_shader_resource_view_desc,
                data //const std::vector < uint8_t >&in_data
            );
        }

#endif
        // Multi line shader and geometry
        {
            std::vector < D3D12_INPUT_ELEMENT_DESC > input_element_desc_array;
            input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
                {
                    "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
                });
                //DXGI_FORMAT_R32G32_UINT
            input_element_desc_array.push_back(D3D12_INPUT_ELEMENT_DESC
                {
                    "TEXCOORD", 0, DXGI_FORMAT_R32G32_SINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, \
                        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 // UINT InstanceDataStepRate;
                }
            );

            // std::shared_ptr<Shader> _multi_line_shader;
            {

                auto vertex_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "multi_line_vertex.cso");
                auto pixel_shader_data = FileSystem::SyncReadFile(in_application_param._data_path / "multi_line_pixel.cso");
                std::vector < DXGI_FORMAT > render_target_format;
                render_target_format.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);
                const auto blend_desc = ShaderPipelineStateData::FactoryBlendDescAlphaPremultiplied();
                ShaderPipelineStateData shader_pipeline_state_data(
                    input_element_desc_array,
                    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
                    DXGI_FORMAT_UNKNOWN,
                    // DXGI_FORMAT_D32_FLOAT,
                    render_target_format,
                    blend_desc,
                    CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),

                    CD3DX12_DEPTH_STENCIL_DESC()// CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
                );

                std::vector<std::shared_ptr<ShaderResourceInfo>> array_shader_resource_info;
                //std::shared_ptr<std::vector<uint8_t>> compute_shader_data;
                //std::vector<std::shared_ptr<UnorderedAccessInfo>> array_unordered_access_info;

#if defined(DSC_UNORDED_ACCESS)
                array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                    _draw_resources->_multi_line_data_pos_thick->GetShaderViewHeapWrapperItem(),
                    D3D12_SHADER_VISIBILITY_VERTEX
                ));
                array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                    _draw_resources->_multi_line_data_dir_length->GetShaderViewHeapWrapperItem(),
                    D3D12_SHADER_VISIBILITY_VERTEX
                ));
                array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                    _draw_resources->_multi_line_data_colour->GetShaderViewHeapWrapperItem(),
                    D3D12_SHADER_VISIBILITY_VERTEX
                ));
#else
                array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                    _draw_resources->_multi_line_data_pos_thick->GetHeapWrapperItem(),
                    D3D12_SHADER_VISIBILITY_VERTEX
                ));
                array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                    _draw_resources->_multi_line_data_dir_length->GetHeapWrapperItem(),
                    D3D12_SHADER_VISIBILITY_VERTEX
                ));
                array_shader_resource_info.push_back(ShaderResourceInfo::FactoryNoSampler(
                    _draw_resources->_multi_line_data_colour->GetHeapWrapperItem(),
                    D3D12_SHADER_VISIBILITY_VERTEX
                ));
#endif

                std::vector<std::shared_ptr<ConstantBufferInfo>> array_shader_constants_info;
                array_shader_constants_info.push_back(ConstantBufferInfo::Factory(
                    ConstantBufferB0(),
                    D3D12_SHADER_VISIBILITY_ALL
                    ));

                _draw_resources->_multi_line_shader = _draw_system->MakeShader(
                    command_list->GetCommandList(),
                    shader_pipeline_state_data,
                    vertex_shader_data,
                    nullptr,
                    pixel_shader_data,
                    array_shader_resource_info,
                    array_shader_constants_info//,
                    //compute_shader_data,
                    //array_unordered_access_info
                );
                _draw_resources->_multi_line_shader_constant_buffer = _draw_resources->_multi_line_shader->MakeShaderConstantBuffer(_draw_system.get());
            }

            // std::shared_ptr<GeometryGeneric> _multi_line_geometry;
            {
                std::vector<uint8_t> vertex_data;
                int trace = 0;
                for (int y = 0; y < 4; ++y)
                {
                    for (int x = 0; x < 4; ++x)
                    {
                        ++trace;
                        if (12 < trace)
                        {
                            break;
                        }
                        //const float uv_x = (1.0f + (x * 2.0f)) / 8.0f;
                        //const float uv_y = (1.0f + (y * 2.0f)) / 8.0f;

                        //a
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                        //b
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                        //c
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);

                        //c
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                        //b
                        VectorHelper::AppendValue<float>(vertex_data, -1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                        //d
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<float>(vertex_data, 1.0f);
                        VectorHelper::AppendValue<int>(vertex_data, x);
                        VectorHelper::AppendValue<int>(vertex_data, y);
                    }
                    if (12 < trace)
                    {
                        break;
                    }
                }
                _draw_resources->_multi_line_geometry = _draw_system->MakeGeometryGeneric(
                    command_list->GetCommandList(),
                    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                    input_element_desc_array,
                    vertex_data,
                    4
                );
            }
        }

    }

    _timer = std::make_unique<Timer>();
}

ApplicationMultiLine::~ApplicationMultiLine()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }

    _draw_resources.reset();
    _draw_system.reset();

    LOG_MESSAGE(
        "ApplicationMultiLine  dtor %p",
        this
        );
}

void ApplicationMultiLine::Update()
{
    BaseType::Update();

    if (nullptr != _timer)
    {
        const float time_delta = _timer->GetDeltaSeconds();
        const auto camera_right = Cross(_camera_at, _camera_up);

        //LOG_MESSAGE("ApplicationMultiLine::Update %f %f %f", _camera_pos[0], _camera_pos[1], _camera_pos[2]);

        if (true == _input_q)
        {
            _camera_pos -= (_camera_up * time_delta);
        }
        if (true == _input_w)
        {
            _camera_pos += (_camera_at * time_delta);
        }
        if (true == _input_e)
        {
            _camera_pos += (_camera_up * time_delta);
        }
        if (true == _input_a)
        {
            _camera_pos -= (camera_right * time_delta);
        }
        if (true == _input_s)
        {
            _camera_pos -= (_camera_at * time_delta);
        }
        if (true == _input_d)
        {
            _camera_pos += (camera_right * time_delta);
        }

        bool rotation_flag = false;
        auto rotation = QuaternionFloat::FactoryIdentity();
        if (true == _input_u)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_camera_at, time_delta);
        }
        if (true == _input_i)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(camera_right, -time_delta);
        }
        if (true == _input_o)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_camera_at, -time_delta);
        }
        if (true == _input_j)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_camera_up, -time_delta);
        }
        if (true == _input_k)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(camera_right, time_delta);
        }
        if (true == _input_l)
        {
            rotation_flag = true;
            rotation *= QuaternionFloat::FactoryAxisAngle(_camera_up, time_delta);
        }

        if (true == rotation_flag)
        {
            const auto matrix = MatrixFloat33::FactoryQuaternionFloat(rotation);
            //matrix.in
            _camera_at = matrix * _camera_at;
            _camera_up = matrix * _camera_up;

            // Normalise
            _camera_at.NormaliseSelf();
            const auto temp = Cross(_camera_at, _camera_up);
            _camera_up = Cross(temp, _camera_at);
            _camera_up.NormaliseSelf();
        }
    }

    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        // Draw background
#if 1
        if ((nullptr != _draw_resources->_background_shader) && (nullptr != _draw_resources->_background_shader_constant_buffer))
        {
            auto& buffer0 = _draw_resources->_background_shader_constant_buffer->GetConstant<ConstantBufferB0>(0);
            buffer0._camera_at = _camera_at;
            buffer0._camera_up = _camera_up;
            buffer0._camera_pos = _camera_pos;
            buffer0._camera_far = _camera_far;
            buffer0._camera_fov_horizontal = _fov_horizontal_calculated;
            buffer0._camera_fov_vertical = _fov_vertical;
            buffer0._camera_unit_pixel_size = _unit_pixel_size;

            auto& buffer_background1 = _draw_resources->_background_shader_constant_buffer->GetConstant<ConstantBufferBackgroundB1>(1);

            buffer_background1._sun_azimuth_altitude = VectorFloat2(Angle::DegToRadian(45.0f), Angle::DegToRadian(45.0f));
            buffer_background1._sun_range = VectorFloat2(0.05f, 0.2f);//1.0f, 5.0f);
            buffer_background1._sun_tint = VectorFloat3(255.0f / 255.0f, 245.0f / 255.0f, 235.0f / 255.0f);
            buffer_background1._sky_spread = -0.75f; //-0.5
            buffer_background1._sky_tint = VectorFloat3(10.0f / 255.0f, 10.0f / 255.0f, 255.0f / 255.0f);
            buffer_background1._sky_turbitity = 25.0f; //5.0f; //10.0f;
            buffer_background1._ground_tint = VectorFloat3(32.0f / 255.0f, 16.0f / 255.0f, 2.0f / 255.0f);
            buffer_background1._fog_tint = VectorFloat3(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f);

            frame->SetShader(_draw_resources->_background_shader, _draw_resources->_background_shader_constant_buffer);
            frame->Draw(_draw_resources->_background_geometry);
        }
#endif

        // Draw Grid
#if 1
        if ((nullptr != _draw_resources->_background_shader_grid) && (nullptr != _draw_resources->_background_shader_grid_constant_buffer))
        {
            auto& buffer0 = _draw_resources->_background_shader_grid_constant_buffer->GetConstant<ConstantBufferB0>(0);
            buffer0._camera_at = _camera_at;
            buffer0._camera_up = _camera_up;
            buffer0._camera_pos = _camera_pos;
            buffer0._camera_far = _camera_far;
            buffer0._camera_fov_horizontal = _fov_horizontal_calculated;
            buffer0._camera_fov_vertical = _fov_vertical;
            buffer0._camera_unit_pixel_size = _unit_pixel_size;

            frame->SetShader(_draw_resources->_background_shader_grid, _draw_resources->_background_shader_grid_constant_buffer);
            frame->Draw(_draw_resources->_background_geometry);
        }
#endif

        // Draw Multi Line
#if 1
        if ((nullptr != _draw_resources->_multi_line_shader) && (nullptr != _draw_resources->_multi_line_shader_constant_buffer))
        {
            frame->ResourceBarrier(_draw_resources->_multi_line_data_pos_thick.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            frame->ResourceBarrier(_draw_resources->_multi_line_data_dir_length.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            frame->ResourceBarrier(_draw_resources->_multi_line_data_colour.get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

            auto& buffer0 = _draw_resources->_multi_line_shader_constant_buffer->GetConstant<ConstantBufferB0>(0);
            buffer0._camera_at = _camera_at;
            buffer0._camera_up = _camera_up;
            buffer0._camera_pos = _camera_pos;
            buffer0._camera_far = _camera_far;
            buffer0._camera_fov_horizontal = _fov_horizontal_calculated;
            buffer0._camera_fov_vertical = _fov_vertical;
            buffer0._camera_unit_pixel_size = _unit_pixel_size;
            buffer0._camera_radian_per_pixel = _radian_per_pixel;

            frame->SetShader(_draw_resources->_multi_line_shader, _draw_resources->_multi_line_shader_constant_buffer);
            frame->Draw(_draw_resources->_multi_line_geometry);
        }
#endif

    }
}

void ApplicationMultiLine::OnWindowSizeChanged(
    const int in_width,
    const int in_height
    )
{
    BaseType::OnWindowSizeChanged(
        in_width,
        in_height
        );

    const float aspect = (float)in_width / (float)in_height;
    _fov_horizontal_calculated = aspect * _fov_vertical;
    _radian_per_pixel = _fov_vertical / (float)in_height;
    _unit_pixel_size = sin(_radian_per_pixel);

    if (_draw_system)
    {
        _draw_system->OnResize();
    }

    return;
}

void ApplicationMultiLine::OnKey(
    const int in_vk_code,
    const int in_scan_code,
    const bool in_repeat_flag,
    const int in_repeat_count,
    bool in_up_flag
    )
{
    in_scan_code;
    in_repeat_flag;
    in_repeat_count;

    if ('Q' == in_vk_code)
    {
        _input_q = (false == in_up_flag);
    }
    else if ('W' == in_vk_code)
    {
        _input_w = (false == in_up_flag);
    }
    else if ('E' == in_vk_code)
    {
        _input_e = (false == in_up_flag);
    }
    else if ('A' == in_vk_code)
    {
        _input_a = (false == in_up_flag);
    }
    else if ('S' == in_vk_code)
    {
        _input_s = (false == in_up_flag);
    }
    else if ('D' == in_vk_code)
    {
        _input_d = (false == in_up_flag);
    }

    else if ('U' == in_vk_code)
    {
        _input_u = (false == in_up_flag);
    }
    else if ('I' == in_vk_code)
    {
        _input_i = (false == in_up_flag);
    }
    else if ('O' == in_vk_code)
    {
        _input_o = (false == in_up_flag);
    }
    else if ('J' == in_vk_code)
    {
        _input_j = (false == in_up_flag);
    }
    else if ('K' == in_vk_code)
    {
        _input_k = (false == in_up_flag);
    }
    else if ('L' == in_vk_code)
    {
        _input_l = (false == in_up_flag);
    }
    else if ((VK_HOME == in_vk_code) && (false == in_up_flag))
    {
        _camera_pos = s_camera_pos;
        _camera_at = s_camera_at;
        _camera_up = s_camera_up;
    }

    return;
}
