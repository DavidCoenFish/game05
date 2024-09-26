#include "static_lq/static_lq_pch.h"
#include "static_lq/combat/simple/simple_combat_output.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/i_combat_side.h"
#include "common/log/log.h"
#include "common/locale/locale_system.h"
#include "common/locale/i_locale_data_provider.h"

namespace
{
    constexpr char s_locale_key_combatant_added[] = "combatant_added";

    class LocaleDataProvider : public ILocaleDataProvider
    {
    private:
        void Populate(LocaleSystem& in_out_locale_system, const LocaleISO_639_1 in_locale) const override
        {
            in_locale;

            const std::vector<LocaleSystem::Data> data = {
                {s_locale_key_combatant_added, "Combatant {combatant} added to side {side}"}};

            in_out_locale_system.Append(in_locale, data);
        }
    };
}


void static_lq::SimpleCombatOutput::RegisterLocaleSystem(LocaleSystem& in_locale_system)
{
    in_locale_system.RegisterProvider(std::make_shared<LocaleDataProvider>());
}

static_lq::SimpleCombatOutput::SimpleCombatOutput(const FCallback& in_log, const std::shared_ptr<LocaleSystem>& in_locale_system)
: _log(in_log)
, _locale_system(in_locale_system)
{
    // nop
}

void static_lq::SimpleCombatOutput::CombatantAdded(const ICombatant& combatant, const ICombatSide& side)
{
    //const std::string message = Log::Printf("CombatantAdded [%d] [%d]\n", combatant.GetId(), side.GetId());
    //m_log(message);
}

void static_lq::SimpleCombatOutput::CombatantRemoved(const ICombatant& combatant, const ICombatSide& side)
{
    const std::string message = Log::Printf("CombatantRemoved [%d] [%d]\n", combatant.GetId(), side.GetId());
    _log(message);
}

void static_lq::SimpleCombatOutput::SetTurnSegment(const int turn, const int segment)
{
    const std::string message = Log::Printf("Turn[%d] Segment[%d]\n", turn, segment);
    _log(message);
}

void static_lq::SimpleCombatOutput::SetCombatEnd(const std::vector<std::shared_ptr<ICombatSide>>& sides_able_to_continue)
{
    const std::string message = Log::Printf("SetCombatEnd sides_able_to_continue.size[%d]\n", sides_able_to_continue.size());
    _log(message);
}
