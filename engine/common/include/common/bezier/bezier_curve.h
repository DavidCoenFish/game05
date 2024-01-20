#pragma once

#include "common/math/vector_int2.h"
#include "common/math/vector_float2.h"

class BezierCurvesImplementation;
class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class Shader;
class VectorInt2;

/// multiple bezier line segments
/// rather than have the thickness in the shader constants, add the start end thickness to the vertex data
class BezierCurve
{
public:
    struct BezierSegment
    {
        /// Start point
        VectorFloat2 _p0; 
        /// Simplified control point, rather than have a _c0 and _c1
        VectorFloat2 _p1; 
        /// End point
        VectorFloat2 _p2;
        /// in pixels, 0.5 and under starts to have difficulty displaying. 
        /// the shader that renders the curve is a little simplistic, so conservative thickness values are supported. 
        /// shader finds the closest point on the curve for zero thickness, then adds thickness
        float _line_thickness_p0;
        //float _line_thickness_p1;
        float _line_thickness_p2;

        const bool operator== (const BezierSegment& in_rhs) const;
        const bool operator!= (const BezierSegment& in_rhs) const;
    };

    BezierCurve(
        const std::vector<BezierSegment>& in_data = std::vector<BezierSegment>(),
        const VectorInt2& in_container_size = VectorInt2::s_zero,
        const VectorInt2& in_container_offset = VectorInt2::s_zero
        );

    ~BezierCurve();

    /// Get the natural size required by the text, if width limit is enabled, when uses width limit
    VectorInt2 GetCurveBounds();

    const bool SetContainerSize(
        const VectorInt2& in_container_size = VectorInt2::s_zero,
        const VectorInt2& in_container_offset = VectorInt2::s_zero
        );

    const bool SetData(
        const std::vector<BezierSegment>& in_data = std::vector<BezierSegment>()
        );

    const bool ModifyData(
        const BezierSegment& in_data,
        const int in_index
        );

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        std::shared_ptr<Shader>& in_shader
        );

private:
    std::unique_ptr<BezierCurvesImplementation> _implementation;

};
