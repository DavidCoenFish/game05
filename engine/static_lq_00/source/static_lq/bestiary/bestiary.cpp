#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/bestiary.h"

#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_enum.h"
#include "common/log/log.h"
#include "static_lq/bestiary/monster_data.h"
#include "static_lq/bestiary/special_characteristic_data.h"
#include "static_lq/combat/combat_enum.h"
#include "static_lq/combat/i_combatant.h"
#include "static_lq/combat/simple/simple_combatant_monster.h"
#include "static_lq/name/name_system.h"
#include "static_lq/random_sequence.h"

namespace
{
    constexpr char s_locale_key_species_name[] = "slqsc_bestiary_species_name";
    constexpr char s_locale_key_species_name_variation[] = "slqsc_bestiary_species_name_variation";
    constexpr char s_locale_key_species_giant_ant[] = "slqsc_bestiary_species_giant_ant";
    constexpr char s_locale_key_species_giant_spider[] = "slqsc_bestiary_species_giant_spider";
    constexpr char s_locale_key_attack_mandibles[] = "slqsc_bestiary_attack_mandibles";
    constexpr char s_locale_key_attack_bite[] = "slqsc_bestiary_attack_bite";
    constexpr char s_locale_key_example_below_average[] = "slqsc_bestiary_example_below_average";
    constexpr char s_locale_key_example_average[] = "slqsc_bestiary_example_average";
    constexpr char s_locale_key_example_above_average[] = "slqsc_bestiary_example_above_average";
    constexpr char s_locale_key_example_exceptional[] = "slqsc_bestiary_example_exceptional";
    constexpr char s_locale_key_example_worker_ant[] = "slqsc_bestiary_example_worker_ant";
    constexpr char s_locale_key_example_warrior_ant[] = "slqsc_bestiary_example_warrior_ant";
    constexpr char s_locale_key_example_dew_pot_worker_ant[] = "slqsc_bestiary_example_dew_pot_worker_ant";
    constexpr char s_locale_key_example_queen_ant[] = "slqsc_bestiary_example_queen_ant";

    //constexpr char s_locale_key_damage_tolerance[] = "slqsc_damage_tolerance";
    //constexpr char s_locale_key_damage_tolerance_tooltip[] = "slqsc_damage_tolerance_tooltip";
    //constexpr char s_locale_key_damage_sum[] = "slqsc_damage_sum";
    //constexpr char s_locale_key_damage_sum_tooltip[] = "slqsc_damage_sum_tooltip";
    //constexpr char s_locale_key_fatigue_damage[] = "slqsc_fatigue_damage";
    //constexpr char s_locale_key_physical_damage[] = "slqsc_physical_damage";
    //constexpr char s_locale_key_paralyzation_damage[] = "slqsc_paralyzation_damage";
    //constexpr char s_locale_key_alive[] = "slqsc_alive";
    //constexpr char s_locale_key_alive_tooltip[] = "slqsc_alive_tooltip";
    //constexpr char s_dag_key_attack_bonus[] = "slqsc_attack_bonus";
    //constexpr char s_dag_key_attack_bonus_tooltip[] = "slqsc_attack_bonus_tooltip";
    //
    //constexpr char s_dag_key_damage_tolerance_dice_constant[] = "damage_tolerance_dice_constant";
    //constexpr char s_dag_key_damage_tolerance_dice_count[] = "damage_tolerance_dice_count";
    //constexpr char s_dag_key_damage_tolerance_dice_sides[] = "damage_tolerance_dice_sides";
    //constexpr char s_dag_key_damage_tolerance_raw[] = "damage_tolerance_raw";
    //constexpr char s_dag_key_fatigue_damage[] = "fatigue_damage";
    //constexpr char s_dag_key_physical_damage[] = "physical_damage";
    //constexpr char s_dag_key_paralyzation_damage[] = "paralyzation_damage";
    //constexpr char s_dag_key_damage_sum_raw[] = "damage_sum_raw";
    //constexpr char s_dag_key_alive[] = "alive";

    void DagAddName(DagThreadedCollection& in_dag_collection, const std::string& in_name_key, const std::string& in_species, const std::string& in_variation)
    {
        in_dag_collection.CreateNodeVariable(
            EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TDisplayName),
            DagThreadedHelper::CreateDagValue<std::string>(in_name_key)
            );
        in_dag_collection.CreateNodeVariable(
            EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TSpecies),
            DagThreadedHelper::CreateDagValue<std::string>(in_species)
            );
        in_dag_collection.CreateNodeVariable(
            EnumSoftBind<StaticLq::CombatEnum::CombatantValue>::EnumToString(StaticLq::CombatEnum::CombatantValue::TVariation),
            DagThreadedHelper::CreateDagValue<std::string>(in_variation)
            );
            // TDisplayNameToolTip0
            // TDisplayNameToolTip1
            // TDisplayNameToolTip2
    }

    void DagAddDamageTolerance(DagThreadedCollection& in_dag_collection, const int32_t in_id, const StaticLq::MonsterVariationData& in_monster_variation_data)
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
                StaticLq::RandomSequence random_sequence(id);
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

    std::shared_ptr<DagThreadedCollection> MakeMonsterDag(const int32_t in_id, const StaticLq::MonsterData& in_monster_data, const int32_t in_variation_index, const std::string& in_name_key)
    {
        //StaticLq::RandomSequence random_sequence(in_id);
        std::shared_ptr<DagThreadedCollection> result = DagThreadedCollection::Factory();

        DagAddName(in_name_key, in_monster_data._species, in_monster_data._array_variation[in_variation_index]._display_name);

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

/// somewhere for locale data for the factory default combatabts to be specified
void StaticLq::Bestiary::RegisterLocaleSystem(LocaleSystem& in_out_locale_system)
{
    const std::vector<LocaleSystem::Data> data = {
        {s_locale_key_species_name, "{species}"},
        {s_locale_key_species_name_variation, "{species} ({variation})"},
        {s_locale_key_species_giant_ant, "Giant Ant"},
        {s_locale_key_species_giant_spider, "Giant Spider"},
        {s_locale_key_example_below_average, "Below Average"},
        {s_locale_key_example_average, "Average"},
        {s_locale_key_example_above_average, "Above Average"},
        {s_locale_key_example_exceptional, "Exceptional"},
        {s_locale_key_example_worker_ant, "Worker Ant"},
        {s_locale_key_example_warrior_ant, "Warrior Ant"},
        {s_locale_key_example_dew_pot_worker_ant, "Dew-Pot Worker Ant"},
        {s_locale_key_example_queen_ant, "Queen Ant"},

        {s_locale_key_attack_mandibles, "mandibles"},
        {s_locale_key_attack_bite, "bite"},

        {s_locale_key_damage_tolerance_tooltip, "{self} = {index.1} + {index.2}d{index.3}"},
        {s_locale_key_damage_sum_tooltip, "{self} = {index.1} + {index.2} + {index.3}"},
        {s_locale_key_alive_tooltip, "{self} = 0 <= {index.0}"},
        };

    in_out_locale_system.Append(LocaleISO_639_1::Default, data);
}

/*
tome_of_terrors.pdf page:142
*/
std::shared_ptr<StaticLq::ICombatant> StaticLq::Bestiary::FactoryDefaultGiantSpider(
    NameSystem& in_name_system, 
    LocaleSystem& in_locale_system 
    )
{
    const MonsterData monster_data = {
        s_locale_key_species_giant_spider,
        static_cast<BestiaryEnum::Habitat>(
            static_cast<int32_t>(BestiaryEnum::Habitat::TForest) | 
            static_cast<int32_t>(BestiaryEnum::Habitat::TSubterranean) | 
            static_cast<int32_t>(BestiaryEnum::Habitat::TDesert)
            ),
        static_cast<BestiaryEnum::Lifestyle>(
            static_cast<int32_t>(BestiaryEnum::Lifestyle::TInstinctive) | 
            static_cast<int32_t>(BestiaryEnum::Lifestyle::TArachnid)
            ),
        BestiaryEnum::WealthType::TIncidental,
        BestiaryEnum::Alignment::TNeutral,
        BestiaryEnum::Cunning::TLow,
        60,
        BestiaryEnum::Strength::TBelowAverage,
        BestiaryEnum::Size::TMedium,
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
                s_locale_key_example_below_average,
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
                        std::vector<AttackEffect>({StaticLq::AttackEffect::TModerateParalyzingVenom})
                    }
                })
            }),
            MonsterVariationData({
                s_locale_key_example_average,
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
                        std::vector<AttackEffect>({StaticLq::AttackEffect::TModerateParalyzingVenom})
                    }
                })
            }),
            MonsterVariationData({
                s_locale_key_example_above_average,
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
                        std::vector<AttackEffect>({StaticLq::AttackEffect::TModerateParalyzingVenom})
                    }
                })
            }),
            MonsterVariationData({
                s_locale_key_example_above_average,
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
                        std::vector<AttackEffect>({StaticLq::AttackEffect::TModerateParalyzingVenom})
                    }
                })
            })
        })
    };

    const int id = ICombatant::MakeNewId();
    const std::string name_key = in_name_system.GenerateName(StaticLq::NameSystem::GetKeyGiantSpider(), id, in_locale_system);

    std::shared_ptr<DagThreadedCollection> dag_collection = MakeMonsterDag(id, monster_data, 1, name_key);

    return std::make_shared<SimpleCombatMonster>(id, dag_collection);
}

/*
tome_of_terrors.pdf page:32
*/
std::shared_ptr<StaticLq::ICombatant> StaticLq::Bestiary::FactoryDefaultGiantAnt(
    NameSystem& in_name_system, 
    LocaleSystem& in_locale_system 
    )
{
    const MonsterData monster_data = {
        s_locale_key_species_giant_ant,
        BestiaryEnum::Habitat::TEverywhere,
        static_cast<BestiaryEnum::Lifestyle>(
            static_cast<int32_t>(BestiaryEnum::Lifestyle::TInstinctive) | 
            static_cast<int32_t>(BestiaryEnum::Lifestyle::TComunal) | 
            static_cast<int32_t>(BestiaryEnum::Lifestyle::TInsect)
            ),
        BestiaryEnum::WealthType::TMineral,
        BestiaryEnum::Alignment::TNeutral,
        BestiaryEnum::Cunning::TLow,
        70,
        BestiaryEnum::Strength::THigh,
        BestiaryEnum::Size::TSmall,
        {},
        7,
        6,
        0,
        {
            std::optional<int32_t>(6),
            std::optional<int32_t>(6),
            std::optional<int32_t>(0),
            std::nullopt,
            std::optional<int32_t>(0),
            std::optional<int32_t>(-3),
            std::optional<int32_t>(-3)
        },
        std::vector<MonsterVariationData>({
            MonsterVariationData({
                s_locale_key_example_worker_ant,
                1,
                3,
                21,
                {15, 1, 10},
                15,
                std::vector<MonsterAttackData>({
                    {
                        s_locale_key_attack_mandibles,
                        {0,2,4},
                        true,
                        {}
                    }
                })
            }),
            MonsterVariationData({
                s_locale_key_example_warrior_ant,
                4,
                6,
                22,
                {15, 4, 10},
                35,
                std::vector<MonsterAttackData>({
                    {
                        s_locale_key_attack_mandibles,
                        {0,5,4},
                        true,
                        std::vector<AttackEffect>({StaticLq::AttackEffect::TModerateParalyzingVenom})
                    }
                })
            }),
            MonsterVariationData({
                s_locale_key_example_dew_pot_worker_ant,
                0,
                0,
                5,
                {10, 1, 10},
                1,
                std::vector<MonsterAttackData>({
                    {
                        s_locale_key_attack_mandibles,
                        {0,2,4},
                        true,
                        {}
                    }
                })
                // set speed 10
            }),
            MonsterVariationData({
                s_locale_key_example_queen_ant,
                0,
                0,
                5,
                {80, 2, 10},
                2,
                std::vector<MonsterAttackData>({
                    {
                        s_locale_key_attack_mandibles,
                        {0,2,4},
                        true,
                        {}
                    }
                })
                // set speed 0
            })
        })
    };

    const int id = ICombatant::MakeNewId();
    const std::string name_key = in_name_system.GenerateName(StaticLq::NameSystem::GetKeyGiantAnt(), id, in_locale_system);

    std::shared_ptr<DagThreadedCollection> dag_collection = MakeMonsterDag(id, monster_data, 1);

    return std::make_shared<SimpleCombatMonster>(id, name_key, dag_collection);
}
