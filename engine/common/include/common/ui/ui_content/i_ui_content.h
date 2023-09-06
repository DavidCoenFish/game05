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
    //template<typename CLASS>
    //static const int ClassTypeID()
    //{
    //    static int s_value = 0;
    //    s_value ++;
    //    return s_value;
    //}
    //static const int GetNewTypeID();

    IUIContent();
    virtual ~IUIContent() = 0;

    // Needed to check IUIContent* was only the default type IUIContent and nothing derrived 
    // Move default implementation into UIContentDefault, and made IUIContent pure
    //virtual const int GetClassTypeID() const = 0;

    virtual const bool GetClearBackground(
        VectorFloat4& out_clear_colour
        ) const = 0;

    //virtual const bool GetNeedsDraw() const = 0;
    //virtual void SetDrawn() = 0;

    virtual const bool UpdateHierarchy(
        std::vector<std::shared_ptr<UIHierarchyNodeChildData>>& out_child_data_array,
        const std::vector<std::shared_ptr<IUIData>>& in_data_array,
        const UIHierarchyNodeUpdateHierarchyParam& in_param
        ) const = 0;

    // Warning, returning a reference, trying to avoid copy. alternative include visitor, change id 
    //virtual const std::vector<std::shared_ptr<IUIData>>& GetArrayData() const = 0;

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

    virtual void Draw(
        const UIManagerDrawParam& in_param,
        Shader* const in_shader,
        UIGeometry* const in_geometry,
        const std::shared_ptr<HeapWrapperItem>& in_heap_wrapper_item
        ) = 0;

};
