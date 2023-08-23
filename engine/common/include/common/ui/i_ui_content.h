#pragma once

class DrawSystem;
class DrawSystemFrame;
class Shader;
class UITexture;
class VectorInt2;
class VectorFloat4;
struct UIHierarchyNodeChildData;
struct UIManagerDrawData;

struct UIManagerUpdateLayoutParam;
struct UIManagerDealInputParam;


class IUIContent
{
public:
    IUIContent();
    virtual ~IUIContent();

    virtual const bool GetClearBackground(
        VectorFloat4& out_clear_colour
        ) const;

    virtual const bool GetNeedsDraw() const;

    //virtual void UpdateLayout(
    //    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_out_child_data_array,
    //    const UIManagerUpdateLayoutParam& in_param,
    //    const std::string& in_model_key
    //    );

    //// so, what owns state of current focus, what click stated on
    //virtual void DealInput(
    //    const UIManagerDealInputParam& in_param
    //    );

    virtual void Draw(
        DrawSystemFrame* const in_frame,
        UITexture* const in_texture,
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
        Shader* const in_shader
        );

};
