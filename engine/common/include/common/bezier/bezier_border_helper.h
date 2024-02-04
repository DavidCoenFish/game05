#pragma once

#include "common/bezier/bezier_curve.h"

namespace BezierBorderHelper
{
    void GenerateCurledUpSegment(
        std::vector<BezierCurve::BezierSegment>& out_segment_data, 
        const std::vector<BezierCurve::BezierSegment>& in_reference_segment_data
        );

    void GenerateSegmentBorder0(
        std::vector<BezierCurve::BezierSegment>& out_segment_data, 
        const VectorInt2& in_inner_size,
        const int in_border_left,
        const int in_border_top,
        const int in_border_right,
        const int in_border_bottom,
        const int in_thickness_min,
        const int in_thickness_max,
        const VectorInt2& in_offset
        );

}

