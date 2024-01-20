#include "common/common_pch.h"
#include "common/bezier/bezier_curve.h"

#include "common/bezier/bezier_manager.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/render_target_texture.h"
#include "common/draw_system/shader/shader.h"
#include "common/math/dsc_math.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"
#include "common/util/vector_helper.h"

namespace
{
    void CalculateMinMax(
        VectorFloat2& out_min,
        VectorFloat2& out_max,
        const BezierCurve::BezierSegment& in_data
        )
    {
        out_min = VectorFloat2(in_data._p0.GetX() - in_data._line_thickness_p0, in_data._p0.GetY() - in_data._line_thickness_p0);
        out_max = VectorFloat2(in_data._p0.GetX() + in_data._line_thickness_p0, in_data._p0.GetY() + in_data._line_thickness_p0);

        const float min_thickness = 0.5f * (in_data._line_thickness_p0 + in_data._line_thickness_p2);
        out_min = VectorFloat2::Min(out_min, 
            VectorFloat2(in_data._p1.GetX() - min_thickness, in_data._p1.GetY() - min_thickness));
        out_max = VectorFloat2::Max(out_max, 
            VectorFloat2(in_data._p1.GetX() + min_thickness, in_data._p1.GetY() + min_thickness));

        out_min = VectorFloat2::Min(out_min, 
            VectorFloat2(in_data._p2.GetX() - in_data._line_thickness_p2, in_data._p2.GetY() - in_data._line_thickness_p2));
        out_max = VectorFloat2::Max(out_max, 
            VectorFloat2(in_data._p2.GetX() + in_data._line_thickness_p2, in_data._p2.GetY() + in_data._line_thickness_p2));

        return;
    }

    void AddVertexData(
        std::vector<uint8_t>& out_vertex_raw_data,
        const float in_uv0,
        const float in_uv1,
        const float in_pos0,
        const float in_pos1,
        const VectorFloat2& in_p0,
        const VectorFloat2& in_p1,
        const VectorFloat2& in_p2,
        const float in_thickness0,
        const float in_thickness2,
        const VectorFloat2& in_width_height,
        const float in_offset_x,
        const float in_offset_y
        )
    {
        //pos
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos0);
        VectorHelper::AppendValue(out_vertex_raw_data, in_pos1);
        //p0 the start
        //VectorHelper::AppendValue(out_vertex_raw_data, in_p0);
        VectorHelper::AppendValue(out_vertex_raw_data, (in_p0.GetX() - in_offset_x) / in_width_height.GetX());
        VectorHelper::AppendValue(out_vertex_raw_data, (in_p0.GetY() - in_offset_y) / in_width_height.GetY());

        //p1 a combined gradient target for both p0 and p1
        //VectorHelper::AppendValue(out_vertex_raw_data, in_p1);
        VectorHelper::AppendValue(out_vertex_raw_data, (in_p1.GetX() - in_offset_x) / in_width_height.GetX());
        VectorHelper::AppendValue(out_vertex_raw_data, (in_p1.GetY() - in_offset_y) / in_width_height.GetY());

        //p2 the end
        //VectorHelper::AppendValue(out_vertex_raw_data, in_p2);
        VectorHelper::AppendValue(out_vertex_raw_data, (in_p2.GetX() - in_offset_x) / in_width_height.GetX());
        VectorHelper::AppendValue(out_vertex_raw_data, (in_p2.GetY() - in_offset_y) / in_width_height.GetY());

        // thickness at p0, p1
        VectorHelper::AppendValue(out_vertex_raw_data, in_thickness0);
        VectorHelper::AppendValue(out_vertex_raw_data, in_thickness2);
        // dim, the pixel width/height that the quad is drawing to
        VectorHelper::AppendValue(out_vertex_raw_data, in_width_height);
        // uv, the uv in the quad to work out the sample point. position can be rahter arbitary so can not base uv on pos
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv0);
        VectorHelper::AppendValue(out_vertex_raw_data, in_uv1);
    }

    void GenerateGeometry(
        std::vector<uint8_t>& out_vertex_raw_data,
        const VectorInt2& in_container_size,
        const VectorInt2& in_container_offset,
        const std::vector<BezierCurve::BezierSegment>& in_data
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

            const VectorFloat2 width_height(
                temp_max.GetX() - temp_min.GetX(), 
                temp_max.GetY() - temp_min.GetY()
                );

            const VectorFloat4 pos = VectorFloat4(
                DscMath::ConvertZeroOneToNegativeOneOne((static_cast<float>(in_container_offset.GetX()) + temp_min.GetX()) / static_cast<float>(in_container_size.GetX())),
                DscMath::ConvertZeroOneToNegativeOneOne((static_cast<float>(in_container_offset.GetY()) + temp_min.GetY()) / static_cast<float>(in_container_size.GetY())),
                DscMath::ConvertZeroOneToNegativeOneOne((static_cast<float>(in_container_offset.GetX()) + temp_max.GetX()) / static_cast<float>(in_container_size.GetX())),
                DscMath::ConvertZeroOneToNegativeOneOne((static_cast<float>(in_container_offset.GetY()) + temp_max.GetY()) / static_cast<float>(in_container_size.GetY()))
                );

            //0.0f, 0.0f,
            AddVertexData(out_vertex_raw_data, 0.0f, 0.0f, pos[0], pos[1], iter._p0, iter._p1, iter._p2, iter._line_thickness_p0, iter._line_thickness_p2, width_height, temp_min.GetX(), temp_min.GetY()); 

            //0.0f, 1.0f,
            AddVertexData(out_vertex_raw_data, 0.0f, 1.0f, pos[0], pos[3], iter._p0, iter._p1, iter._p2, iter._line_thickness_p0, iter._line_thickness_p2, width_height, temp_min.GetX(), temp_min.GetY()); 

            //1.0f, 0.0f,
            AddVertexData(out_vertex_raw_data, 1.0f, 0.0f, pos[2], pos[1], iter._p0, iter._p1, iter._p2, iter._line_thickness_p0, iter._line_thickness_p2, width_height, temp_min.GetX(), temp_min.GetY()); 

            //1.0f, 0.0f,
            AddVertexData(out_vertex_raw_data, 1.0f, 0.0f, pos[2], pos[1], iter._p0, iter._p1, iter._p2, iter._line_thickness_p0, iter._line_thickness_p2, width_height, temp_min.GetX(), temp_min.GetY()); 

            //0.0f, 1.0f,
            AddVertexData(out_vertex_raw_data, 0.0f, 1.0f, pos[0], pos[3], iter._p0, iter._p1, iter._p2, iter._line_thickness_p0, iter._line_thickness_p2, width_height, temp_min.GetX(), temp_min.GetY()); 

            //1.0f, 1.0f,
            AddVertexData(out_vertex_raw_data, 1.0f, 1.0f, pos[2], pos[3], iter._p0, iter._p1, iter._p2, iter._line_thickness_p0, iter._line_thickness_p2, width_height, temp_min.GetX(), temp_min.GetY()); 

        }
    }
};

const bool BezierCurve::BezierSegment::operator== (const BezierSegment& in_rhs) const
{
    if (_p0 != in_rhs._p0)
    {
        return false;
    }
    if (_p1 != in_rhs._p1)
    {
        return false;
    }
    if (_p2 != in_rhs._p2)
    {
        return false;
    }
    if (_line_thickness_p0 != in_rhs._line_thickness_p0)
    {
        return false;
    }
    if (_line_thickness_p2 != in_rhs._line_thickness_p2)
    {
        return false;
    }

    return true;
}

const bool BezierCurve::BezierSegment::operator!= (const BezierSegment& in_rhs) const
{
    return !(operator==(in_rhs));
}

class BezierCurvesImplementation
{
public:
    BezierCurvesImplementation(
        const std::vector<BezierCurve::BezierSegment>& in_data,
        const VectorInt2& in_container_size,
        const VectorInt2& in_container_offset
        )
        : _calculate_dirty(true)
        , _geometry_dirty(true)
        , _geometry()
        , _container_size(in_container_size)
        , _container_offset(in_container_offset)
        , _data(in_data)
    {
        // Nop
    }

    // Get the natural bounds required by the bezier curve
    VectorInt2 GetCurveBounds()
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

    const bool SetContainerSize(
        const VectorInt2& in_container_size = VectorInt2::s_zero,
        const VectorInt2& in_container_offset = VectorInt2::s_zero
        )
    {
        bool dirty = false;
        if ((_container_size != in_container_size) ||
            (_container_offset != in_container_offset))
        {
            dirty = true;
            _geometry_dirty = true;
            _container_size = in_container_size;
            _container_offset = in_container_offset;
        }
        return dirty;
    }

    const bool SetData(
        const std::vector<BezierCurve::BezierSegment>& in_data
        )
    {
        bool dirty = false;
        if (_data != in_data)
        {
            dirty = true;
            _calculate_dirty = true;
            _geometry_dirty = true;
        }
        return dirty;
    }

    const bool ModifyData(
        const BezierCurve::BezierSegment& in_data,
        const int in_index
        )
    {
        bool dirty = false;
        if ((0 <= in_index) && (in_index < static_cast<int>(_data.size())))
        {
            BezierCurve::BezierSegment& segment = _data[in_index];
            if (segment != in_data)
            {
                segment = in_data;
                dirty = true;
                _calculate_dirty = true;
                _geometry_dirty = true;
            }
        }
        return dirty;
    }

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        std::shared_ptr<Shader>& in_shader
        )
    {
        GetCurveBounds();

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
                    14
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
    VectorInt2 _container_offset;
    std::vector<BezierCurve::BezierSegment> _data;

};

BezierCurve::BezierCurve(
    const std::vector<BezierSegment>& in_data,
    const VectorInt2& in_container_size,
    const VectorInt2& in_container_offset
    )
{
    _implementation = std::make_unique<BezierCurvesImplementation>(
        in_data,
        in_container_size,
        in_container_offset
        );
}

BezierCurve::~BezierCurve()
{
    // nop
}

/// Get the natural size required by the text, if width limit is enabled, when uses width limit
VectorInt2 BezierCurve::GetCurveBounds()
{
    return _implementation->GetCurveBounds();
}

const bool BezierCurve::SetContainerSize(
    const VectorInt2& in_container_size,
    const VectorInt2& in_container_offset
    )
{
    return _implementation->SetContainerSize(
        in_container_size,
        in_container_offset
        );
}

const bool BezierCurve::SetData(
    const std::vector<BezierSegment>& in_data
    )
{
    return _implementation->SetData(
        in_data
        );
}

const bool BezierCurve::ModifyData(
    const BezierSegment& in_data,
    const int in_index
    )
{
    return _implementation->ModifyData(
        in_data,
        in_index
        );
}

void BezierCurve::Draw(
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
