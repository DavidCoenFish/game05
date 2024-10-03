#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/combat_time.h"

StaticLq::CombatTime::CombatTime(
    const int32_t in_turn,
    const int32_t in_segment
    )
    : _turn(in_turn)
    , _segment(in_segment)
{
    // nop
}

StaticLq::CombatTime::~CombatTime()
{
    // nop
}

void StaticLq::CombatTime::AdvanceTime()
{
    _segment += 1;
    while (10 < _segment)
    {
        _turn += 1;
        _segment -= 10;
    }
}

const bool StaticLq::CombatTime::IsStartOfTurn() const
{
    return (1 == _segment);
}

