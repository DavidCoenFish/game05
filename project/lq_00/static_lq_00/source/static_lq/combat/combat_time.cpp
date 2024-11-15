#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/combat_time.h"

StaticLq::CombatTime::CombatTime(
	const int32_t in_turn,
	const int32_t in_segment
	)
	: _turn(in_turn)
	, _segment(in_segment)
{
	Wrap();
}

StaticLq::CombatTime::~CombatTime()
{
	// nop
}

void StaticLq::CombatTime::AdvanceTime()
{
	_segment += 1;
	Wrap();
}

const bool StaticLq::CombatTime::IsStartOfTurn() const
{
	return (1 == _segment);
}

const bool StaticLq::CombatTime::Equivalent(const CombatTime& in_lhs, const CombatTime& in_rhs)
{
	if (in_lhs._turn != in_rhs._turn)
	{
		return false;
	}
	if (in_lhs._segment != in_rhs._segment)
	{
		return false;
	}
	return true;
}

StaticLq::CombatTime StaticLq::CombatTime::Add(const CombatTime& in_lhs, const int32_t in_turn_delta, const int32_t in_segment_delta)
{
	return CombatTime(
		in_lhs.GetTurn() + in_turn_delta,
		in_lhs.GetSegment() + in_segment_delta
		); 
}

void StaticLq::CombatTime::Wrap()
{
	while (10 < _segment)
	{
		_turn += 1;
		_segment -= 10;
	}
}
