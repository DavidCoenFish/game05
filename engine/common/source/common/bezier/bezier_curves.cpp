#include "common/common_pch.h"
#include "common/bezier/bezier_curves.h"

#include "common/bezier/bezier_manager.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/draw_system/shader/shader.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"

namespace
{
    void CalculateMinMax(
        VectorFloat2& out_min,
        VectorFloat2& out_max,
        const BezierCurves::BezierSegment& in_data
        )
    {
        VectorFloat2 low(in_data._p0.GetX() - in_data._line_thickness_p0, in_data._p0.GetY() - in_data._line_thickness_p0);
        VectorFloat2 high(in_data._p0.GetX() + in_data._line_thickness_p0, in_data._p0.GetY() + in_data._line_thickness_p0);

        const float min_thickness = 0.5f * (in_data._line_thickness_p0 + in_data._line_thickness_p2);
        low = VectorFloat2::Min(low, 
            VectorFloat2(in_data._p1.GetX() - min_thickness, in_data._p1.GetY() - min_thickness));
        high = VectorFloat2::Max(high, 
            VectorFloat2(in_data._p1.GetX() + min_thickness, in_data._p1.GetY() + min_thickness));

        low = VectorFloat2::Min(low, 
            VectorFloat2(in_data._p2.GetX() - in_data._line_thickness_p2, in_data._p2.GetY() - in_data._line_thickness_p2));
        high = VectorFloat2::Max(high, 
            VectorFloat2(in_data._p2.GetX() + in_data._line_thickness_p2, in_data._p2.GetY() + in_data._line_thickness_p2));

        return;
    }

    void GenerateGeometry(
        std::vector<uint8_t>& out_vertex_raw_data,
        const VectorInt2& in_containter_size,
        const VectorInt2& in_containter_offset,
        const std::vector<BezierCurves::BezierSegment>& in_data
        )
    {
        for (auto& iter : in_data)
        {
            VectorFloat2 temp_min;
            VectorFloat2 temp_max;

            CalculateMinMax(
                temp_min,
                temp_max,
                iter
                );

            if (temp_min == temp_max)
            {
                continue;
            }


        }
    }
};

class BezierCurvesImplementation
{
public:
    BezierCurvesImplementation(
        const std::vector<BezierCurves::BezierSegment>& in_data,
        const VectorInt2& in_containter_size,
        const VectorInt2& in_containter_offset
        )
        : _calculate_dirty(true)
        , _geometry_dirty(true)
        , _geometry()
        , _container_size(in_containter_size)
        , _containter_offset(in_containter_offset)
        , _data(in_data)
    {
        // Nop
    }

    // Get the natural size required by the text using current width limit if enabled
    VectorInt2 GetBounds()
    {
        if (true == _calculate_dirty)
        {
            _calculate_dirty = false;

            VectorFloat2 low(VectorFloat2::s_max);
            VectorFloat2 high(VectorFloat2::s_min);
            bool valid = false;

            for (auto& iter : _data)
            {
                valid = true;

                VectorFloat2 temp_min;
                VectorFloat2 temp_max;
                CalculateMinMax(
                    temp_min,
                    temp_max,
                    iter
                    );

                // Can vaugly recall seeing IQ solve this properly, but this will work for now
                low = VectorFloat2::Min(low, temp_min);
                high = VectorFloat2::Max(high, temp_max);
            }

            if (true == valid)
            {
                _bounds = VectorInt2(
                    static_cast<int>(std::ceilf(high.GetX() - low.GetX())),
                    static_cast<int>(std::ceilf(high.GetY() - low.GetY()))
                    );
            }
            else
            {
                _bounds = VectorInt2::s_zero;
            }
        }

        return _bounds;
    }

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        std::shared_ptr<Shader>& in_shader
        )
    {
        GetBounds();

        if (true == _geometry_dirty)
        {
            _geometry_dirty = false;

            std::vector<uint8_t> vertex_raw_data;
            GenerateGeometry(
                vertex_raw_data,
                _container_size,
                _container_offset,
                _data
                );

            // the problem with resizing an existing geometry, is what if that geometry is still on a command list
            // we can modify the data, as that just pokes different data onto the command list, but resize can end up being destructive, so better is to destroy and recreate the geometry if size changes
            // again, found by fps text doing something like "0.0" => "59.9"
            if ((nullptr != _geometry) && (vertex_raw_data.size() != _geometry->GetVertexDataByteSize()))
            {
                // note, the DrawSystem may still be holding a reference to the shared pointer if the geometry is still on an in use command list
                _geometry.reset();
            }

            if (nullptr == _geometry)
            {
                _geometry = in_draw_system->MakeGeometryGeneric(
                    in_draw_system_frame->GetCommandList(),
                    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
                    BezierManager::GetInputElementDesc(),
                    vertex_raw_data,
                    6 //12
                    );
            }
            else
            {
                in_draw_system->UpdateGeometryGeneric(
                    in_draw_system_frame->GetCommandList(),
                    _geometry.get(),
                    vertex_raw_data
                    );
            }
        }

        in_draw_system_frame->SetShader(in_shader);
        in_draw_system_frame->Draw(_geometry);
    }

private:
    bool _calculate_dirty;
    bool _geometry_dirty;
    std::shared_ptr<GeometryGeneric> _geometry;
    /// the rounded up max width and height of the bezier point, just using the point position, don't include origin (0,0)
    VectorInt2 _bounds;
    VectorInt2 _container_size;
    VectorInt2 _containter_offset;
    std::vector<BezierCurves::BezierSegment> _data;

};

BezierCurves::BezierCurves(
    const std::vector<BezierSegment>& in_data,
    const VectorInt2& in_containter_size,
    const VectorInt2& in_containter_offset
    )
{
    _implementation = std::make_unique<BezierCurvesImplementation>(
        in_data,
        in_containter_size,
        in_containter_offset
        );
}

BezierCurves::~BezierCurves()
{
    // nop
}

/// Get the natural size required by the text, if width limit is enabled, when uses width limit
VectorInt2 BezierCurves::GetCurveBounds()
{
    return _implementation->GetCurveBounds();
}

const bool BezierCurves::SetContainerSize(
    const VectorInt2& in_containter_size,
    const VectorInt2& in_containter_offset
    )
{
    return _implementation->SetContainerSize(
        in_containter_size,
        in_containter_offset
        );
}

const bool BezierCurves::SetData(
    const std::vector<BezierSegment>& in_data
    )
{
    return _implementation->SetData(
        in_data
        );
}

const bool BezierCurves::ModifyData(
    const BezierSegment& in_data,
    const int in_index
    )
{
    return _implementation->ModifyData(
        in_data,
        in_index
        );
}

void BezierCurves::Draw(
    DrawSystem* const in_draw_system,
    DrawSystemFrame* const in_draw_system_frame,
    std::shared_ptr<Shader>& in_shader
    )
{
    _implementation->Draw(
        in_draw_system,
        in_draw_system_frame,
        in_shader
        );
    return;
}
