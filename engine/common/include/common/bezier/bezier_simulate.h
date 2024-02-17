#pragma once

#include "common/bezier/bezier_curve.h"

class BezierSimulateImplementation;

/// make a 2d "spring" system based on a set of bezier segments. able to set target, advance time, and extract simulation data as bezier segment data
class BezierSimulate
{
public:
    /// could split out input segment data into a SetTargetHard call, but trying to construct  "ready to use" object
    BezierSimulate(
        const std::vector<BezierCurve::BezierSegment>& in_segment_data = {},
        const float in_spring_force = 1.0f,
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

    void Update(const float in_time_delta);

    void GatherData(
        std::vector<BezierCurve::BezierSegment>& out_segment_data
        ) const;

private:
    std::unique_ptr<BezierSimulateImplementation> _implementation;

};

