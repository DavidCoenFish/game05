#pragma once

#include "common/math/vector_int2.h"
#include "common/math/vector_float2.h"
#include "common/bezier/bezier_curve.h"

class BezierCurvesImplementation;
class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;
class HeapWrapperItem;
class Shader;
class VectorInt2;

/// multiple bezier line segments
/// rather than have the thickness in the shader constants, add the start end thickness to the vertex data
class BezierSimulate
{
public:
    /// could split out input segment data into a SetTargetHard call, but trying to construct ready to use
    BezierSimulate(
        const std::vector<BezierCurve::BezierSegment>& in_segment_data = {},
        const float in_sprint_force = 1.0f,
        const float in_dampen = 0.1f
        );

    /// Change target to input and move simulation to it over time. adjust simulation to match the number of input segments
    void SetTargetSoft(
        const std::vector<BezierCurve::BezierSegment>& in_segment_data
        );
    /// Adjust simulation to match target, zero velocity
    void SetTargetHard(
        const std::vector<BezierCurve::BezierSegment>& in_segment_data
        );

    void Tick(const float in_time_delta);

    void GatherData(
        std::vector<BezierCurve::BezierSegment>& out_segment_data
        );

}

