#pragma once

namespace static_lq
{
namespace bestiary_enum
{
enum class Habitat
{
    TDefault = 0,
    TEverywhere,

};
NLOHMANN_JSON_SERIALIZE_ENUM( Habitat, {
    {Habitat::TDefault, nullptr},
    {Habitat::TEverywhere, "everywhere"},
});

enum class Lifestyle
{
    TDefault = 0,
    TComunal = 1 << 0,
    TInstinctive = 1 << 1,
    TInsect = 1 << 2
};
NLOHMANN_JSON_SERIALIZE_ENUM( Lifestyle, {
    {Lifestyle::TDefault, nullptr},
    {Lifestyle::TComunal, "comunal"},
    {Lifestyle::TInstinctive, "instinctive"},
    {Lifestyle::TInsect, "insect"},
});

//_wealthType : LegendaryQuest.Bestiary.Enum.s_wealth.TMineral,

enum class WealthType
{
    TDefault = 0,
    TMineral = 1 << 0,
};
NLOHMANN_JSON_SERIALIZE_ENUM( WealthType, {
    {WealthType::TDefault, nullptr},
    {WealthType::TMineral, "mineral"},
});

//_alignment :  LegendaryQuest.Bestiary.Enum.s_alignment.TNeutral,
enum class Alignment
{
    TDefault = 0,
    TNeutral,
};
NLOHMANN_JSON_SERIALIZE_ENUM( Alignment, {
    {Alignment::TDefault, nullptr},
    {Alignment::TNeutral, "neutral"},
});

//_cunning :  LegendaryQuest.Bestiary.Enum.s_cunning.TLow,
enum class Cunning
{
    TDefault = 0,
    TLow,
};
NLOHMANN_JSON_SERIALIZE_ENUM( Cunning, {
    {Cunning::TDefault, nullptr},
    {Cunning::TLow, "low"},
});


//_strength : LegendaryQuest.Bestiary.Enum.s_strength.THigh,
enum class Strength
{
    TDefault = 0,
    THigh,
};
NLOHMANN_JSON_SERIALIZE_ENUM( Strength, {
    {Strength::TDefault, nullptr},
    {Strength::THigh, "high"},
});


//_size : LegendaryQuest.Bestiary.Enum.s_size.TMedium,
enum class Size
{
    TDefault = 0,
    TMedium,
};
NLOHMANN_JSON_SERIALIZE_ENUM( Size, {
    {Size::TDefault, nullptr},
    {Size::TMedium, "medium"},
});



}
}

/*
#include "common/util/enum_soft_bind.h"


template <> 
const LocaleISO_639_1 EnumSoftBind<LocaleISO_639_1>::EnumFromString(const std::string& in_locale)
{
    const auto& map = GetStringEnumMap();
    auto found = map.find(in_locale);
    if (found != map.end())
    {
        return found->second;
    }
    return LocaleISO_639_1::Default;
}

template <> 
const std::string EnumSoftBind<LocaleISO_639_1>::EnumToString(const LocaleISO_639_1 in_locale)
{
    const auto& map = GetEnumStringMap();
    auto found = map.find(in_locale);
    if (found != map.end())
    {
        return found->second;
    }
    return s_default_data;
}


*/