#pragma once

#include "common/ui/ui_component/i_ui_component.h"
#include "common/ui/ui_component/ui_component_default.h"

class DrawSystemFrame;
class HeapWrapperItem;
class Shader;
class UIGeometry;
class UITexture;
struct UIHierarchyNodeChildData;

class UIComponentTexture : public IUIComponent
{
public:
    UIComponentTexture(
        const UIBaseColour& in_base_colour,
        const UILayout& in_layout,
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
        );
    virtual ~UIComponentTexture();

    void SetTexture(
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
        );

private:
    //virtual void Draw(
    //    const UIManagerDrawParam& in_param,
    //    UITexture* const in_texture,
    //    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
    //    Shader* const in_shader
    //    ) override;

private:
    UIComponentDefault _content_default;

    bool _dirty;
    std::shared_ptr<HeapWrapperItem> _shader_resource_view_handle;
    std::shared_ptr<UIGeometry> _geometry;

    //std::shared_ptr<ShaderResoure> _texture;
    //std::shared_ptr<Shader> _shader;
};
