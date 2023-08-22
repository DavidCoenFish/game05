#pragma once

#include "common/ui/i_ui_content.h"

class DrawSystemFrame;
class HeapWrapperItem;
class Shader;
class UIGeometry;
class UITexture;
struct UIHierarchyNodeChildData;


class UIContentTexture : public IUIContent
{
public:
    UIContentTexture(
        DrawSystem* const in_draw_system,
        ID3D12GraphicsCommandList* const in_command_list,
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
        );
    virtual ~UIContentTexture();

    void SetTexture(
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
        );

private:
    virtual const bool GetNeedsDraw() const override;

    virtual void Draw(
        DrawSystemFrame* const in_frame,
        UITexture* const in_texture,
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
        Shader* const in_shader
        ) override;

private:
    bool _dirty;
    std::shared_ptr<HeapWrapperItem> _shader_resource_view_handle;
    std::shared_ptr<UIGeometry> _geometry;

    //std::shared_ptr<ShaderResoure> _texture;
    //std::shared_ptr<Shader> _shader;
};
