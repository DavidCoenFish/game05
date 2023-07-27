#pragma once

class GeometryGeneric;
class DrawSystem;

/*
    rather than [-1 ... 1], if we follow a [0 ... 1] convention then we have an easier time building quad data
*/
class SceneComponentScreenQuad
{
public:
    static std::shared_ptr<SceneComponentScreenQuad> Factory(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list
        );
    SceneComponentScreenQuad(
        const std::shared_ptr<GeometryGeneric>& in_geometry
        );
    ~SceneComponentScreenQuad();

    GeometryGeneric* const GetGeometry() const;
    static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDescArray();

private:
    std::shared_ptr<GeometryGeneric> _geometry;
    //std::vector<D3D12_INPUT_ELEMENT_DESC> _input_element_desc_array;

};
