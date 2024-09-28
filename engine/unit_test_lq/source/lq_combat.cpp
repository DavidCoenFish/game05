#include "unit_test_lq_pch.h"
#include "static_lq\combat\combat.h"

#include "common\locale\locale_system.h"
#include "common\locale\locale_enum.h"
#include "static_lq\combat\simple\simple_combat_output.h"
#include "static_lq\combat\simple\simple_combat_side.h"
#include "static_lq\combat\simple\simple_combat_topology.h"
#include "static_lq\bestiary\bestiary.h"

#include "static_lq\name\name_system.h"
#include "static_lq\name\name_system_generator_random.h"
#include "static_lq\random_sequence.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LqCombat
{
    // make a combat of a default giant spider and default giant ant, run combat
	TEST_CLASS(Intergration)
	{
	public:
		TEST_METHOD(DefaultCombat)
		{
            std::shared_ptr<LocaleSystem> locale_system = std::make_shared<LocaleSystem>();
            static_lq::SimpleCombatOutput::RegisterLocaleSystem(*locale_system);
            std::shared_ptr<static_lq::NameSystem> name_system = std::make_shared<static_lq::NameSystem>();
            name_system->AddGenerator(static_lq::NameSystem::GetKeySide(), static_lq::NameSystemGeneratorRandom::FactoryGeneratorSide());
            name_system->AddGenerator(static_lq::NameSystem::GetKeyGiantAnt(), static_lq::NameSystemGeneratorRandom::FactoryGeneratorGiantAnt());
            name_system->AddGenerator(static_lq::NameSystem::GetKeyGiantSpider(), static_lq::NameSystemGeneratorRandom::FactoryGeneratorGiantSpider());
            static_lq::NameSystem::RegisterLocaleSystem(name_system, *locale_system);
            locale_system->SetLocaleAndPopulate(LocaleISO_639_1::English);

            std::shared_ptr<static_lq::ICombatOutput> combat_output = std::make_shared<static_lq::SimpleCombatOutput>([](const std::string& message)
            {
                Logger::WriteMessage(message.c_str());
            }, locale_system);

            std::shared_ptr<static_lq::RandomSequence> random_sequence = std::make_shared<static_lq::RandomSequence>(0);
            std::shared_ptr<static_lq::ICombatTopology> combatant_topology = std::make_shared<static_lq::SimpleCombatTopology>();

            static_lq::Combat combat(
                combat_output,
                random_sequence,
                combatant_topology
                );


            // add side giant ants
            {
                std::vector<std::shared_ptr<static_lq::ICombatant>> combatant_array;
                combatant_array.push_back(static_lq::Bestiary::FactoryDefaultGiantAnt(*name_system, *locale_system));
                combatant_array.push_back(static_lq::Bestiary::FactoryDefaultGiantAnt(*name_system, *locale_system));
                combatant_array.push_back(static_lq::Bestiary::FactoryDefaultGiantAnt(*name_system, *locale_system));
                combatant_array.push_back(static_lq::Bestiary::FactoryDefaultGiantAnt(*name_system, *locale_system));
                std::shared_ptr<static_lq::ICombatSide> side = static_lq::SimpleCombatSide::Factory(*name_system, *locale_system, combatant_array);
                combat.AddSide(side);
            }

            // add side giant spider
            {
                std::vector<std::shared_ptr<static_lq::ICombatant>> combatant_array;
                combatant_array.push_back(static_lq::Bestiary::FactoryDefaultGiantSpider(*name_system, *locale_system));
                combatant_array.push_back(static_lq::Bestiary::FactoryDefaultGiantSpider(*name_system, *locale_system));
                std::shared_ptr<static_lq::ICombatSide> side = static_lq::SimpleCombatSide::Factory(*name_system, *locale_system, combatant_array);
                combat.AddSide(side);
            }

            while(true == combat.AdvanceTime())
            {
                //nop
            }
		}

    };
}
