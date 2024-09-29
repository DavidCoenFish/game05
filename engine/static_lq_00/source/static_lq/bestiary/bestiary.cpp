#include "static_lq/static_lq_pch.h"
#include "static_lq/bestiary/bestiary.h"

#include "common/dag/threaded/dag_threaded_collection.h"
#include "common/dag/threaded/dag_threaded_helper.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_enum.h"
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

    void MakeHealthDag(DagThreadedCollection& in_dag_collection, const int32_t in_id, const static_lq::MonsterVariationData& in_monster_variation_data)
    {
        
        auto damage_tolerance_constant = DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._damage_tolerance._constant);
        auto damage_dice_count = DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._damage_tolerance._dice_count);
        auto damage_dice_base = DagThreadedHelper::CreateDagValue<int32_t>(in_monster_variation_data._damage_tolerance._dice_base);
        // we could make a calculate node which used the Roll data, so if that was changed, we get a new total?
        //auto damage_tolerance = in_monster_variation_data._damage_tolerance._constant + 

    }

    std::shared_ptr<DagThreadedCollection> MakeMonsterDag(const int32_t in_id, const static_lq::MonsterData& in_monster_data, const int32_t in_variation_index)
    {
        //static_lq::RandomSequence random_sequence(in_id);
        std::shared_ptr<DagThreadedCollection> result = DagThreadedCollection::Factory();

        MakeHealthDag(*result, in_id, in_monster_data._array_variation[in_variation_index]);

        return result;
    }
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
        };

    in_out_locale_system.Append(LocaleISO_639_1::Default, data);
}

/*
tome_of_terrors.pdf page:142
*/
std::shared_ptr<static_lq::ICombatant> static_lq::Bestiary::FactoryDefaultGiantSpider(
    NameSystem& in_name_system, 
    LocaleSystem& in_locale_system,
    const int32_t in_variation_index
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

    if ((in_variation_index < 0) || (monster_data._array_variation.size() <= in_variation_index))
    {
        return nullptr;
    }

    const int id = ICombatant::MakeNewId();
    const std::string name_key = in_name_system.GenerateName(static_lq::NameSystem::GetKeyGiantAnt(), id, in_locale_system);

    std::shared_ptr<DagThreadedCollection> dag_collection = MakeMonsterDag(id, monster_data, in_variation_index);

    return std::make_shared<SimpleCombatMonster>(id, name_key, dag_collection);
}

std::shared_ptr<static_lq::ICombatant> static_lq::Bestiary::FactoryDefaultGiantAnt(
    NameSystem& in_name_system, 
    LocaleSystem& in_locale_system,
    const int32_t in_variation_index
    )
{
    const int id = ICombatant::MakeNewId();
    const std::string name_key = in_name_system.GenerateName(static_lq::NameSystem::GetKeyGiantSpider(), id, in_locale_system);
    return std::make_shared<SimpleCombatMonster>(id, name_key);
}
