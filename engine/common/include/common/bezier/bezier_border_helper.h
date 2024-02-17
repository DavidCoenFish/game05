#pragma once

#include "common/bezier/bezier_curve.h"

class VectorFloat2;

namespace BezierBorderHelper
{
    void GenerateCurledUpSegment(
        std::vector<BezierCurve::BezierSegment>& out_segment_data, 
        const std::vector<BezierCurve::BezierSegment>& in_reference_segment_data
        );

    void GenerateSegmentBorder0(
        std::vector<BezierCurve::BezierSegment>& out_segment_data, 
        const VectorFloat2& in_inner_size,
        const float in_border_left,
        const float in_border_top,
        const float in_border_right,
        const float in_border_bottom,
        const float in_thickness_min,
        const float in_thickness_max,
        const VectorFloat2& in_offset
        );

}

