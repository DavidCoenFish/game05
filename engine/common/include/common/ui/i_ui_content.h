#pragma once

class DrawSystem;
class DrawSystemFrame;
class Shader;
class UITexture;
class VectorInt2;
struct UIHierarchyNodeChildData;
struct UIManagerDrawData;

struct UIManagerDealInputParam;

class IUIContent
{
public:
    IUIContent();
    virtual ~IUIContent();
    
    //virtual const bool GetNeedsUpdateLayout() const;
    virtual const bool GetNeedsDraw() const;

    virtual void UpdateLayout(
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_out_child_data_array,
        UIManagerUpdateLayoutParam& in_param,
        const std::string& in_model_key
        );

    // so, what owns state of current focus, what click stated on
    virtual void DealInput(
        UIManagerDealInputParam& in_param
        );

    virtual void Draw(
        DrawSystemFrame* const in_frame,
        UITexture* const in_texture,
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& in_child_data_array,
        Shader* const in_shader
        );

};
