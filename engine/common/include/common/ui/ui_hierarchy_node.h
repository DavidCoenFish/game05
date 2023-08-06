#pragma once

#include "common/ui/ui_coord.h"

class IUIContent;
class UIGeometry;
class UITexture;

class UIHierarchyNode
{
public:
    struct LayoutData
    {
        // Data for how we calculate our size relative to parent
        UICoord _data_size[2];
        UICoord _data_attach[2];
        UICoord _data_pivot[2];
    };

    UIHierarchyNode(
        const LayoutData& in_layout_data
        );
    ~UIHierarchyNode();

    const LayoutData& GetLayoutData() const { return _layout_data; }

private:
    struct ChildData
    {
        std::unique_ptr<UIHierarchyNode> _node;
        // Need to track if state changed, so not using GeometryGeneric
        //std::unique_ptr<GeometryGeneric> _geometry;
        std::unique_ptr<UIGeometry> _geometry;
    };

    std::vector<std::shared_ptr<ChildData>> _child_data_array;
    std::unique_ptr<IUIContent> _content;
    std::unique_ptr<UITexture> _texture;

    LayoutData _layout_data;

};
