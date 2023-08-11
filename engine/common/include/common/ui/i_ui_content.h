#pragma once

class DrawSystem;
class DrawSystemFrame;
class Shader;
class UITexture;
class VectorInt2;
struct UIHierarchyNodeChildData;

class IUIContent
{
public:
    IUIContent();
    virtual ~IUIContent();
    
    virtual const bool GetNeedsToDraw() const;

    virtual const bool UpdatePosSizeForChildren( // add vectorint2 size for render target to child data?
        DrawSystemFrame* const in_frame,
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
        const VectorInt2& in_parent_size,
        const float in_ui_scale
        );

    // ChildrenDraw
    virtual const bool Update(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_frame,
        Shader* const in_shader,
        const float in_ui_scale,
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array
        );

    virtual void Draw(
        DrawSystemFrame* const in_frame,
        UITexture* const in_texture,
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
        Shader* const in_shader,
        const bool in_allow_clear
        );

    //CalculateChildDimentions
    //Draw
    //OnClick?
    //OnSelect?
    //UpdateChildrenIfRequired?

};
