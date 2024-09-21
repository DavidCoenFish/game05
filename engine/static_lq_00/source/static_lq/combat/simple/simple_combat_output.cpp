#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combat_output.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_side.h"
#include "common/log/log.h"

static_lq::SimpleCombatOutput::SimpleCombatOutput(const FCallback& log)
: m_log(log)
{
    // nop
}

void static_lq::SimpleCombatOutput::CombatantAdded(const ICombatant& combatant, const ICombatSide& side)
{
    const std::string message = Log::Printf("CombatantAdded [%d] [%d]\n", combatant.GetId(), side.GetId());
    m_log(message);
}

void static_lq::SimpleCombatOutput::CombatantRemoved(const ICombatant& combatant, const ICombatSide& side)
{
    const std::string message = Log::Printf("CombatantRemoved [%d] [%d]\n", combatant.GetId(), side.GetId());
    m_log(message);
}

void static_lq::SimpleCombatOutput::SetTurnSegment(const int turn, const int segment)
{
    const std::string message = Log::Printf("Turn[%d] Segment[%d]\n", turn, segment);
    m_log(message);
}

void static_lq::SimpleCombatOutput::SetCombatEnd(const std::vector<std::shared_ptr<ICombatSide>>& sides_able_to_continue)
{
    const std::string message = Log::Printf("SetCombatEnd sides_able_to_continue.size[%d]\n", sides_able_to_continue.size());
    m_log(message);

}
