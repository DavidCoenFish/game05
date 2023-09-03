#pragma once

#include "common/ui/ui_content/ui_content_default.h"

class DrawSystemFrame;
class HeapWrapperItem;
class Shader;
class UIGeometry;
class UITexture;
struct UIHierarchyNodeChildData;

class UIContentTexture : public UIContentDefault
{
public:
    UIContentTexture(
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
        );
    virtual ~UIContentTexture();

    void SetTexture(
        const std::shared_ptr<HeapWrapperItem>& in_shader_resource_view_handle
        );

private:
    virtual const int GetClassTypeID() const override;

    virtual const bool GetNeedsDraw() const override;
    //virtual void SetDrawn() override;

    virtual void Draw(
        const UIManagerDrawParam& in_param,
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
