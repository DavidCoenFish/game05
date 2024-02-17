#include "common/common_pch.h"
#include "common/bezier/bezier_simulate.h"
#include "common/math/dsc_math.h"

namespace
{
    enum class SegmentData
    {
        P0,
        P1,
        P2,
        TCount
    };

    struct OtherPointsSpring
    {
    public:
        int _point_index;
        float _target_distance;
    };

    struct Point
    {
    public:
        VectorFloat2 _position;
        VectorFloat2 _velocity;
        /// 
        int _segment_index;
        SegmentData _segment_data;

        float _thickness;

        VectorFloat2 _target;
        std::vector<OtherPointsSpring> _other_point_springs;
    };

    /// ref google: The spring force formula is expressed through the equation: F = – kx 
    void AppendSpringForce(
        VectorFloat2& in_out_force_sum, 
        const VectorFloat2& in_lhs, 
        const VectorFloat2& in_rhs, 
        const float in_target_distance, 
        const float in_spring_force
        )
    {
        VectorFloat2 delta = in_rhs - in_lhs;
        const float approx_distance = DscMath::ApproximateDistance(delta.GetX(), delta.GetY());
        const float x = in_target_distance - approx_distance;
        const float f = (-in_spring_force) * x;
        if (false == DscMath::AlmostEqual(approx_distance, 0.0f))
        {
            in_out_force_sum = in_out_force_sum + (delta * (f / approx_distance));
        }
        return;
    }

    void BuildPoints(std::vector<Point>& out_point_array, const std::vector<BezierCurve::BezierSegment>& in_segment_data)
    {
        
    }
};

class BezierSimulateImplementation
{
public:
    BezierSimulateImplementation(
        const std::vector<BezierCurve::BezierSegment>& in_segment_data,
        const float in_spring_force,
        const float in_dampen
        )
        : _spring_force(in_spring_force)
        , _dampen(in_dampen)
    {
        SetTargetHard(in_segment_data);
    }

    void SetTargetSoft(
        const std::vector<BezierCurve::BezierSegment>& in_segment_data
        )
    {
        _segment_data_size = static_cast<int>(in_segment_data.size());
        const auto copy_points = _point_array;
        BuildPoints(_point_array, in_segment_data);

        // copy the point position and velocity, this is a bit approx if the soft target has a different number of segments
        const int count = std::min(_point_array.size(), copy_points.size());
        for (int index = 0; index < count; ++index)
        {
            auto& dest_point = _point_array[index];
            const auto& source_point = copy_points[index];
            dest_point._position = source_point._position;
            dest_point._velocity = source_point._velocity;
        }

        return;
    }

    void SetTargetHard(
        const std::vector<BezierCurve::BezierSegment>& in_segment_data
        )
    {
        _segment_data_size = static_cast<int>(in_segment_data.size());
        BuildPoints(_point_array, in_segment_data);
    }

    void Update(const float in_time_delta)
    {
        if (true == DscMath::AlmostEqual(0.0f, in_time_delta))
        {
            return;
        }

        const float time_step_dampen = pow(_dampen, in_time_delta);

        for (auto& point : _point_array)
        {
            VectorFloat2 force_sum;

            // spring force to target
            AppendSpringForce(force_sum, point._position, point._target, 0.0f, _spring_force);

            // sprint force to other points
            for (auto& other_point_spring : point._other_point_springs)
            {
                AppendSpringForce(force_sum, point._position, _point_array[other_point_spring._point_index]._target, other_point_spring._target_distance, _spring_force);
            }

            point._velocity = (point._velocity + (force_sum * in_time_delta)) * time_step_dampen;
            point._position = point._position + (point._velocity * in_time_delta);
        }
    }

    void GatherData(
        std::vector<BezierCurve::BezierSegment>& out_segment_data
        ) const
    {
        out_segment_data.resize(_segment_data_size);
        for (const auto& point : _point_array)
        {
            auto& segment = out_segment_data[point._segment_index];
            switch (point._segment_data)
            {
            default:
                DSC_ASSERT_ALWAYS("missing case");
                break;
            case SegmentData::P0:
                segment._p0 = point._position;
                segment._line_thickness_p0 = point._thickness;
                break;
            case SegmentData::P1:
                segment._p1 = point._position;
                break;
            case SegmentData::P2:
                segment._p2 = point._position;
                segment._line_thickness_p2 = point._thickness;
                break;
            }
        }

        return;
    }

private:
    const float _spring_force;
    const float _dampen;
    std::vector<Point> _point_array;
    /// the size of the input, and the size we will use for the output segment_data
    int _segment_data_size;

};

/// could split out input segment data into a SetTargetHard call, but trying to construct  "ready to use" object
BezierSimulate::BezierSimulate(
    const std::vector<BezierCurve::BezierSegment>& in_segment_data,
    const float in_spring_force,
    const float in_dampen
    )
{
    _implementation = std::make_unique<BezierSimulateImplementation>(in_segment_data, in_spring_force, in_dampen);
    return;
}

/// Change target to input and move simulation to it over time. adjust simulation to match the number of input segments
void BezierSimulate::SetTargetSoft(
    const std::vector<BezierCurve::BezierSegment>& in_segment_data
    )
{
    _implementation->SetTargetSoft(in_segment_data);
    return;
}

/// Adjust simulation to match target, zero velocity
void BezierSimulate::SetTargetHard(
    const std::vector<BezierCurve::BezierSegment>& in_segment_data
    )
{
    _implementation->SetTargetHard(in_segment_data);
    return;
}

void BezierSimulate::Update(const float in_time_delta)
{
    _implementation->Update(in_time_delta);
    return;
}

void BezierSimulate::GatherData(
    std::vector<BezierCurve::BezierSegment>& out_segment_data
    ) const
{
    _implementation->GatherData(out_segment_data);
    return;
}

