#pragma once

#include "common/bezier/bezier_curve.h"

namespace BezierFileHelper
{
    const bool SegmentDataFromSvg(
        std::vector<BezierCurve::BezierSegment>& out_segment_data, 
        const std::filesystem::path& in_file_path
        );
}
