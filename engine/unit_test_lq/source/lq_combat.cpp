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
			StaticLq::SimpleCombatOutput::RegisterLocaleSystem(*locale_system);
			StaticLq::Bestiary::RegisterLocaleSystem(*locale_system);
			std::shared_ptr<StaticLq::NameSystem> name_system = std::make_shared<StaticLq::NameSystem>();
			name_system->AddGenerator(StaticLq::NameSystem::GetKeySide(), StaticLq::NameSystemGeneratorRandom::FactoryGeneratorSide());
			name_system->AddGenerator(StaticLq::NameSystem::GetKeyGiantAnt(), StaticLq::NameSystemGeneratorRandom::FactoryGeneratorGiantAnt());
			name_system->AddGenerator(StaticLq::NameSystem::GetKeyGiantSpider(), StaticLq::NameSystemGeneratorRandom::FactoryGeneratorGiantSpider());
			StaticLq::NameSystem::RegisterLocaleSystem(name_system, *locale_system);
			locale_system->SetLocaleAndPopulate(LocaleISO_639_1::English);

			std::shared_ptr<StaticLq::ICombatOutput> combat_output = std::make_shared<StaticLq::SimpleCombatOutput>([](const std::string& message)
			{
				Logger::WriteMessage(message.c_str());
			}, locale_system);

			std::shared_ptr<StaticLq::RandomSequence> random_sequence = std::make_shared<StaticLq::RandomSequence>(0);
			std::shared_ptr<StaticLq::ICombatTopology> combatant_topology = std::make_shared<StaticLq::SimpleCombatTopology>();

			StaticLq::Combat combat(
				combat_output,
				random_sequence,
				combatant_topology
				);


			// add side giant ants
			{
				std::vector<std::shared_ptr<StaticLq::ICombatant>> combatant_array;
				combatant_array.push_back(StaticLq::Bestiary::FactoryDefaultGiantAnt(*name_system, *locale_system));
				combatant_array.push_back(StaticLq::Bestiary::FactoryDefaultGiantAnt(*name_system, *locale_system));
				combatant_array.push_back(StaticLq::Bestiary::FactoryDefaultGiantAnt(*name_system, *locale_system));
				std::shared_ptr<StaticLq::ICombatSide> side = StaticLq::SimpleCombatSide::Factory(*name_system, *locale_system, combatant_array);
				combat.AddSide(side);
			}

			// add side giant spider
			{
				std::vector<std::shared_ptr<StaticLq::ICombatant>> combatant_array;
				combatant_array.push_back(StaticLq::Bestiary::FactoryDefaultGiantSpider(*name_system, *locale_system));
				combatant_array.push_back(StaticLq::Bestiary::FactoryDefaultGiantSpider(*name_system, *locale_system));
				std::shared_ptr<StaticLq::ICombatSide> side = StaticLq::SimpleCombatSide::Factory(*name_system, *locale_system, combatant_array);
				combat.AddSide(side);
			}

			while(true == combat.AdvanceTime())
			{
				//nop
			}
		}

	};
}
