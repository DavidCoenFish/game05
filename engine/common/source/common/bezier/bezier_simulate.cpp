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
		int _point_index = 0;
		float _target_distance = 0.0f;
	};

	struct Point
	{
	public:
		VectorFloat2 _position;
		VectorFloat2 _velocity;
		VectorFloat2 _target;
		std::vector<OtherPointsSpring> _other_point_springs;
	};

	struct SegmentMapping
	{
	public:
		int _point_index_0 = 0;
		int _point_index_1 = 0;
		int _point_index_2 = 0;
		float _thickness_0 = 0.0f;
		float _thickness_2 = 0.0f;
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

	// TODO: replace with hash? else this is an almost O=n^2 operation
	const int FindOrAddPoint(
		std::vector<Point>& out_point_array, 
		const VectorFloat2& in_position
		)
	{
		const int count = static_cast<int>(out_point_array.size());
		for (int index = 0; index < count; ++index)
		{
			const auto& point = out_point_array[index];
			if ((true == DscMath::AlmostEqual(in_position.GetX(), point._position.GetX())) &&
				(true == DscMath::AlmostEqual(in_position.GetY(), point._position.GetY())))
			{
				return index;
			}
		}

		Point add_point;
		add_point._position = in_position;
		add_point._target = in_position;
		out_point_array.push_back(add_point);
		return count;
	}

	void AddSpring(
		std::vector<Point>& in_out_point_array, 
		const int in_p0,
		const int in_p1
		)
	{
		auto& point_0 = in_out_point_array[in_p0];
		auto& point_1 = in_out_point_array[in_p1];
		const auto delta = point_0._position - point_1._position;
		const float distance = DscMath::ApproximateDistance(delta.GetX(), delta.GetY());
		point_0._other_point_springs.push_back(
			OtherPointsSpring({in_p1, distance})
			);
		point_1._other_point_springs.push_back(
			OtherPointsSpring({in_p0, distance})
			);

		return;
	}
	
	void BuildPoints(
		std::vector<Point>& out_point_array, 
		std::vector<SegmentMapping>& out_segment_mapping_array,
		const std::vector<BezierCurve::BezierSegment>& in_segment_data
		)
	{
		out_point_array.resize(0);
		const int count = static_cast<int>(in_segment_data.size());
		out_segment_mapping_array.resize(count);
		for (int index = 0; index < count; ++index)
		{
			const auto& segment = in_segment_data[index];
			auto& segment_mapping = out_segment_mapping_array[index];
			segment_mapping._point_index_0 = FindOrAddPoint(out_point_array, segment._p0);
			segment_mapping._point_index_1 = FindOrAddPoint(out_point_array, segment._p1);
			segment_mapping._point_index_2 = FindOrAddPoint(out_point_array, segment._p2);
			segment_mapping._thickness_0 = segment._line_thickness_p0;
			segment_mapping._thickness_2 = segment._line_thickness_p2;

			AddSpring(out_point_array, segment_mapping._point_index_0, segment_mapping._point_index_1);
			AddSpring(out_point_array, segment_mapping._point_index_0, segment_mapping._point_index_2);
			AddSpring(out_point_array, segment_mapping._point_index_1, segment_mapping._point_index_2);
		}
		return;
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
		const auto copy_points = _point_array;
		BuildPoints(_point_array, _segment_mapping_array, in_segment_data);

		// copy the point position and velocity, this is a bit approx if the soft target has a different number of segments
		const int count = std::min(static_cast<int>(_point_array.size()), static_cast<int>(copy_points.size()));
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
		BuildPoints(_point_array, _segment_mapping_array, in_segment_data);
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
		out_segment_data.resize(_segment_mapping_array.size());
		int trace = 0;
		for (const auto& segment_mapping : _segment_mapping_array)
		{
			const auto& point_0 = _point_array[segment_mapping._point_index_0];
			const auto& point_1 = _point_array[segment_mapping._point_index_1];
			const auto& point_2 = _point_array[segment_mapping._point_index_2];
			auto& segment = out_segment_data[trace];
			trace += 1;
			segment._p0 = point_0._position;
			segment._line_thickness_p0 = segment_mapping._thickness_0;
			segment._p1 = point_1._position;
			segment._p2 = point_2._position;
			segment._line_thickness_p2 = segment_mapping._thickness_2;
		}

		return;
	}

private:
	const float _spring_force;
	const float _dampen;
	std::vector<Point> _point_array;
	std::vector<SegmentMapping> _segment_mapping_array;

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

BezierSimulate::~BezierSimulate()
{
	_implementation.reset();
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

