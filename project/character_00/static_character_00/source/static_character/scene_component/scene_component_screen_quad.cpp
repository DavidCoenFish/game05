#include "static_character/static_character_pch.h"
#include "static_character/scene_component/scene_component_screen_quad.h"

#include "common/draw_system/draw_system.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/util/vector_helper.h"

namespace
{
    static std::vector < D3D12_INPUT_ELEMENT_DESC > s_input_element_desc_array(
        {
            D3D12_INPUT_ELEMENT_DESC
            {
                "POSITION", 
                0, 
                DXGI_FORMAT_R32G32_FLOAT, 
                0, 
                D3D12_APPEND_ALIGNED_ELEMENT,
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
                0 // UINT InstanceDataStepRate;
            }
        }
    );

}

std::shared_ptr<SceneComponentScreenQuad> SceneComponentScreenQuad::Factory(
    DrawSystem* const in_draw_system,
    ID3D12GraphicsCommandList* const in_command_list
    )
{
    const auto vertex_data = VectorHelper::FactoryArrayLiteral(
        {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
        }
    );

    auto geometry = in_draw_system->MakeGeometryGeneric(
        in_command_list,
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
        s_input_element_desc_array,
        vertex_data,
        2
        );

    return std::make_shared<SceneComponentScreenQuad>(
        geometry
        );
}

SceneComponentScreenQuad::SceneComponentScreenQuad(
    const std::shared_ptr<GeometryGeneric>& in_geometry
    )
    : _geometry(in_geometry)
{
    // Nop
}

SceneComponentScreenQuad::~SceneComponentScreenQuad()
{
    // Nop
}

const std::shared_ptr<GeometryGeneric>& SceneComponentScreenQuad::GetGeometryRef() const
{
    return _geometry;
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& SceneComponentScreenQuad::GetInputElementDescArray()
{
    return s_input_element_desc_array;
}
