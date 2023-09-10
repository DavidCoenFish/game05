#pragma once

#include "common/ui/ui_content/i_ui_content.h"

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


class UIContentDefault : public IUIContent
{
public:
    UIContentDefault();
    virtual ~UIContentDefault();

    // These methods are intentionally private, the are intended to provide default implementation, but not be called from derrived
private:

    virtual const bool GetClearBackground(
        VectorFloat4& out_clear_colour
        ) const override;

    virtual void* GetSourceUIDataToken() const override;

    virtual const bool UpdateHierarchy(
        const IUIData* const in_data,
        UIHierarchyNodeChildData& in_out_child_data,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) override;

    //virtual const bool GetNeedsDraw() const override;
    //virtual void SetDrawn() override;

    //virtual const bool UpdateHierarchy(
    //    std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
    //    const IUIData& in_data,
    //    const UIHierarchyNodeUpdateHierarchyParam& in_param
    //    ) const override;

    //// Warning, returning a reference, trying to avoid copy. alternative include visitor, change id 
    //virtual const std::vector<std::shared_ptr<IUIData>>& GetArrayData() const override;

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
        Shader* const in_shader,
        UIGeometry* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item
        ) override;

private:
    void* _source_ui_data_token;

};
