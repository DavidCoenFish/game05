#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/bestiary.h"

#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_enum.h"
#include "common/log/log.h"
#include "static_lq/bestiary/monster_data.h"
#include "static_lq/bestiary/special_characteristic_data.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"
#include "static_lq/name/name_system.h"
#include "static_lq/random_sequence.h"

namespace
{
    constexpr char s_locale_key_species_name[] = "slqsc_bestiary_species_name";
    constexpr char s_locale_key_species_name_variation[] = "slqsc_bestiary_species_name_variation";
    constexpr char s_locale_key_species_giant_spider[] = "slqsc_bestiary_species_giant_spider";
    constexpr char s_locale_key_attack_bite[] = "slqsc_bestiary_attack_bite";
    constexpr char s_locale_key_species_below_average[] = "slqsc_bestiary_species_below_average";
    constexpr char s_locale_key_species_average[] = "slqsc_bestiary_species_average";
    constexpr char s_locale_key_species_above_average[] = "slqsc_bestiary_species_above_average";
    constexpr char s_locale_key_species_exceptional[] = "slqsc_bestiary_species_exceptional";
    constexpr char s_locale_key_damage_tolerance[] = "slqsc_damage_tolerance";
    constexpr char s_locale_key_damage_tolerance_tooltip[] = "slqsc_damage_tolerance_tooltip";
    constexpr char s_locale_key_damage_sum[] = "slqsc_damage_sum";
    constexpr char s_locale_key_damage_sum_tooltip[] = "slqsc_damage_sum_tooltip";
    constexpr char s_locale_key_fatigue_damage[] = "slqsc_fatigue_damage";
    constexpr char s_locale_key_physical_damage[] = "slqsc_physical_damage";
    constexpr char s_locale_key_paralyzation_damage[] = "slqsc_paralyzation_damage";
    constexpr char s_locale_key_alive[] = "slqsc_alive";
    constexpr char s_locale_key_alive_tooltip[] = "slqsc_alive_tooltip";

    constexpr char s_dag_key_damage_tolerance_dice_constant[] = "damage_tolerance_dice_constant";
    constexpr char s_dag_key_damage_tolerance_dice_count[] = "damage_tolerance_dice_count";
    constexpr char s_dag_key_damage_tolerance_dice_sides[] = "damage_tolerance_dice_sides";
    constexpr char s_dag_key_damage_tolerance_raw[] = "damage_tolerance_raw";
    constexpr char s_dag_key_fatigue_damage[] = "fatigue_damage";
    constexpr char s_dag_key_physical_damage[] = "physical_damage";
    constexpr char s_dag_key_paralyzation_damage[] = "paralyzation_damage";
    constexpr char s_dag_key_damage_sum_raw[] = "damage_sum_raw";
    constexpr char s_dag_key_alive[] = "alive";

    void DagAddDamageTolerance(DagThreadedCollection& in_dag_collection, const int32_t in_id, const static_lq::MonsterVariationData& in_monster_variation_data)
    {
        auto damage_tolerance_seed = in_dag_collection.CreateNodeVariable(
            in_dag_collection.MakeUid(), 
            DagThreadedHelper::CreateDagValue<int32_t>(in_id)
            );
        auto damage_tolerance_constant = in_dag_collection.CreateNodeVariable(
            s_dag_key_damage_tolerance_dice_constant, 
            DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._damage_tolerance._constant)
            );
        auto damage_tolerance_dice_count = in_dag_collection.CreateNodeVariable(
            s_dag_key_damage_tolerance_dice_count, 
            DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._damage_tolerance._dice_count)
            );
        auto damage_tolerance_dice_base = in_dag_collection.CreateNodeVariable(
            s_dag_key_damage_tolerance_dice_sides, 
            DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._damage_tolerance._dice_base)
            );

        auto damage_tolerance = in_dag_collection.CreateNodeCalculate(
            s_dag_key_damage_tolerance_raw,
            [](
            	const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
        		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
            ) -> std::shared_ptr< IDagThreadedValue >
            {
                const int32_t id = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
                const int32_t constant = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
                const int32_t dice_count = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[2]);
                const int32_t dice_base = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[3]);
                int32_t result = constant;
                static_lq::RandomSequence random_sequence(id);
                for (int index = 0; index < dice_count; ++index)
                {
                    result += random_sequence.GenerateDice(dice_base);
                }

                return DagThreadedHelper::CreateDagValue<int32_t>(result);
            },
            s_locale_key_damage_tolerance,
            s_locale_key_damage_tolerance_tooltip
            );

        in_dag_collection.AddNodeLinkIndexed(damage_tolerance, damage_tolerance_seed, 0);
        in_dag_collection.AddNodeLinkIndexed(damage_tolerance, damage_tolerance_constant, 1);
        in_dag_collection.AddNodeLinkIndexed(damage_tolerance, damage_tolerance_dice_count, 2);
        in_dag_collection.AddNodeLinkIndexed(damage_tolerance, damage_tolerance_dice_base, 3);
    }

    void DagAddDamageSum(DagThreadedCollection& in_dag_collection)
    {
        auto fatigue_damage = in_dag_collection.CreateNodeVariable(
            s_dag_key_fatigue_damage, 
            DagThreadedHelper::CreateDagValue<int32_t>(0),
            DagThreaded::ValueChanged,
            s_locale_key_fatigue_damage
            );
        auto physical_damage = in_dag_collection.CreateNodeVariable(
            s_dag_key_physical_damage, 
            DagThreadedHelper::CreateDagValue<int32_t>(0),
            DagThreaded::ValueChanged,
            s_locale_key_physical_damage
            );
        auto paralyzation_damage = in_dag_collection.CreateNodeVariable(
            s_dag_key_paralyzation_damage, 
            DagThreadedHelper::CreateDagValue<int32_t>(0),
            DagThreaded::ValueChanged,
            s_locale_key_paralyzation_damage
            );

        auto damage_sum = in_dag_collection.CreateNodeCalculate(
            s_dag_key_damage_sum_raw,
            [](
            	const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
        		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
            ) -> std::shared_ptr< IDagThreadedValue >
            {
                const int32_t fatigue_damage_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
                const int32_t physical_damage_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
                const int32_t paralyzation_damage_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[2]);
                const int32_t sum = fatigue_damage_value + physical_damage_value + paralyzation_damage_value;
                return DagThreadedHelper::CreateDagValue<int32_t>(sum);
            },
            s_locale_key_damage_sum,
            s_locale_key_damage_sum_tooltip
            );

        in_dag_collection.AddNodeLinkIndexed(damage_sum, fatigue_damage, 0);
        in_dag_collection.AddNodeLinkIndexed(damage_sum, physical_damage, 1);
        in_dag_collection.AddNodeLinkIndexed(damage_sum, paralyzation_damage, 2);
    }

    void DagAddAlive(DagThreadedCollection& in_dag_collection)
    {
        auto alive = in_dag_collection.CreateNodeCalculate(
            s_dag_key_alive,
            [](
            	const std::vector< std::shared_ptr< IDagThreadedValue > >&, 
        		const std::vector< std::shared_ptr< IDagThreadedValue > >& in_array_indexed
            ) -> std::shared_ptr< IDagThreadedValue >
            {
                const int32_t damage_tolerance_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[0]);
                const int32_t damage_sum_value = DagThreadedHelper::GetValue<int32_t>(in_array_indexed[1]);
                const bool alive = 0 <damage_tolerance_value - damage_sum_value;
                return DagThreadedHelper::CreateDagValue<bool>(alive);
            },
            s_locale_key_alive,
            s_locale_key_alive_tooltip
            );

        in_dag_collection.AddNodeLinkIndexed(alive, in_dag_collection.FindNode(s_dag_key_damage_tolerance_raw), 0);
        in_dag_collection.AddNodeLinkIndexed(alive, in_dag_collection.FindNode(s_dag_key_damage_sum_raw), 1);
    }

    std::shared_ptr<DagThreadedCollection> MakeMonsterDag(const int32_t in_id, const static_lq::MonsterData& in_monster_data, const int32_t in_variation_index)
    {
        //static_lq::RandomSequence random_sequence(in_id);
        std::shared_ptr<DagThreadedCollection> result = DagThreadedCollection::Factory();

        DagAddDamageTolerance(*result, in_id, in_monster_data._array_variation[in_variation_index]);
        DagAddDamageSum(*result);
        DagAddAlive(*result);

        #if 0
        auto damage_tollerace = result->FindNode(s_dag_key_damage_tolerance_raw);
        const int32_t hp = DagThreadedHelper::GetValue<int32_t>(result->GetDagValue(damage_tollerace));
        LOG_CONSOLE("MakeMonsterDag hp:[%d]", hp);
        #endif

        return result;
    }
}

const std::string static_lq::Bestiary::GetDagKeyAlive()
{
    return s_dag_key_alive;
}

const std::string static_lq::Bestiary::GetDagKeyFatigueDamage()
{
    return s_dag_key_fatigue_damage;
}

const std::string static_lq::Bestiary::GetDagKeyPhysicalDamage()
{
    return s_dag_key_physical_damage;
}

const std::string static_lq::Bestiary::GetDagKeyParalyzationDamage()
{
    return s_dag_key_paralyzation_damage;
}

/// somewhere for locale data for the factory default combatabts to be specified
void static_lq::Bestiary::RegisterLocaleSystem(LocaleSystem& in_out_locale_system)
{
    const std::vector<LocaleSystem::Data> data = {
        {s_locale_key_species_name, "{species}"},
        {s_locale_key_species_name_variation, "{species} ({variation})"},
        {s_locale_key_species_giant_spider, "Giant Spider"},
        {s_locale_key_species_below_average, "Below Average"},
        {s_locale_key_species_average, "Average"},
        {s_locale_key_species_above_average, "Above Average"},
        {s_locale_key_species_exceptional, "Exceptional"},

        {s_locale_key_damage_tolerance, "Damage tolerance"},
        {s_locale_key_damage_tolerance_tooltip, "{self} = {index.1} + {index.2}d{index.3}"},
        {s_locale_key_damage_sum, "Damage sum"},
        {s_locale_key_damage_sum_tooltip, "{self} = {index.1} + {index.2} + {index.3}"},

        {s_locale_key_fatigue_damage, "slqsc_fatigue_damage"},
        {s_locale_key_physical_damage, "slqsc_physical_damage"},
        {s_locale_key_paralyzation_damage, "slqsc_paralyzation_damage"},
        {s_locale_key_alive, "slqsc_alive"},
        {s_locale_key_alive_tooltip, "slqsc_alive_tooltip"},

        };

    in_out_locale_system.Append(LocaleISO_639_1::Default, data);
}

/*
tome_of_terrors.pdf page:142
*/
std::shared_ptr<static_lq::ICombatant> static_lq::Bestiary::FactoryDefaultGiantSpider(
    NameSystem& in_name_system, 
    LocaleSystem& in_locale_system 
    )
{
    const MonsterData monster_data = {
        s_locale_key_species_giant_spider,
        static_cast<bestiary_enum::Habitat>(
            static_cast<int32_t>(bestiary_enum::Habitat::TForest) | 
            static_cast<int32_t>(bestiary_enum::Habitat::TSubterranean) | 
            static_cast<int32_t>(bestiary_enum::Habitat::TDesert)
            ),
        static_cast<bestiary_enum::Lifestyle>(
            static_cast<int32_t>(bestiary_enum::Lifestyle::TInstinctive) | 
            static_cast<int32_t>(bestiary_enum::Lifestyle::TArachnid)
            ),
        bestiary_enum::WealthType::TIncidental,
        bestiary_enum::Alignment::TNeutral,
        bestiary_enum::Cunning::TLow,
        60,
        bestiary_enum::Strength::TBelowAverage,
        bestiary_enum::Size::TMedium,
        {
            {SpecialCharacteristics::TAttackCuresDisease},
            {SpecialCharacteristics::TDarkVision}
        },
        6,
        0,
        2,
        {
            std::optional<int32_t>(-1),
            std::optional<int32_t>(-1),
            std::optional<int32_t>(1),
            std::nullopt,
            std::optional<int32_t>(1),
            std::optional<int32_t>(-4),
            std::optional<int32_t>(-4)
        },
        std::vector<MonsterVariationData>({
            MonsterVariationData({
                s_locale_key_species_below_average,
                1,
                3,
                22,
                {10, 1, 10},
                9,
                std::vector<MonsterAttackData>({
                    {
                        s_locale_key_attack_bite,
                        {0,1,4},
                        false,
                        std::vector<AttackEffect>({static_lq::AttackEffect::TModerateParalyzingVenom})
                    }
                })
            }),
            MonsterVariationData({
                s_locale_key_species_average,
                3,
                5,
                24,
                {10, 3, 10},
                20,
                std::vector<MonsterAttackData>({
                    {
                        s_locale_key_attack_bite,
                        {0,1,4},
                        false,
                        std::vector<AttackEffect>({static_lq::AttackEffect::TModerateParalyzingVenom})
                    }
                })
            }),
            MonsterVariationData({
                s_locale_key_species_above_average,
                5,
                7,
                26,
                {10, 5, 10},
                54,
                std::vector<MonsterAttackData>({
                    {
                        s_locale_key_attack_bite,
                        {0,1,6},
                        false,
                        std::vector<AttackEffect>({static_lq::AttackEffect::TModerateParalyzingVenom})
                    }
                })
            }),
            MonsterVariationData({
                s_locale_key_species_above_average,
                7,
                9,
                28,
                {15, 7, 10},
                162,
                std::vector<MonsterAttackData>({
                    {
                        s_locale_key_attack_bite,
                        {0,1,6},
                        false,
                        std::vector<AttackEffect>({static_lq::AttackEffect::TModerateParalyzingVenom})
                    }
                })
            })
        })
    };

    const int id = ICombatant::MakeNewId();
    const std::string name_key = in_name_system.GenerateName(static_lq::NameSystem::GetKeyGiantSpider(), id, in_locale_system);

    std::shared_ptr<DagThreadedCollection> dag_collection = MakeMonsterDag(id, monster_data, 1);

    return std::make_shared<SimpleCombatMonster>(id, name_key, dag_collection);
}

std::shared_ptr<static_lq::ICombatant> static_lq::Bestiary::FactoryDefaultGiantAnt(
    NameSystem& in_name_system, 
    LocaleSystem& in_locale_system 
    )
{
    const int id = ICombatant::MakeNewId();
    const std::string name_key = in_name_system.GenerateName(static_lq::NameSystem::GetKeyGiantAnt(), id, in_locale_system);

    return std::make_shared<SimpleCombatMonster>(id, name_key, nullptr);
}
