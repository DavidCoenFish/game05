#pragma once

class DrawSystem;
class DrawSystemFrame;
class IUIData;
class Shader;
class UITexture;
class VectorInt2;
class VectorFloat4;

struct UIManagerDrawData;
struct UIManagerDrawParam;
struct UIManagerUpdateLayoutParam;
struct UIManagerDealInputParam;
struct UIHierarchyNodeChildData;
struct UIHierarchyNodeLayoutData;
struct UIHierarchyNodeUpdateHierarchyParam;


class IUIContent
{
public:
    IUIContent();
    virtual ~IUIContent();

    virtual const bool GetClearBackground(
        VectorFloat4& out_clear_colour
        ) const;

    virtual const bool GetNeedsDraw() const;
    virtual void SetDrawn();

    virtual const bool UpdateHierarchy(
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
        const IUIData& in_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        );

    virtual const std::vector<std::shared_ptr<IUIData>> GetArrayData() const;

    // if we are going to use this to auto scroll text, need some way of compunicating uv. layout_data? or the ui_geometry?
    //virtual const bool Update(
    //    VectorInt2& out_texture_size,
    //    UIHierarchyNodeLayoutData& out_layout_data,
    //    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
    //    const VectorInt2& in_parent_size,
    //    const IUIData* const in_data,
    //    const std::string& in_model_key,
    //    const UIHierarchyNodeUpdateLayoutParam& in_param
    //    );

    /*
    passing in_child_data_array as some components may want things from the child
    default implementation is to draw all children onto in_texture
    */
    virtual void Draw(
        const UIManagerDrawParam& in_param,
        UITexture* const in_texture,
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
        Shader* const in_shader
        );

};
