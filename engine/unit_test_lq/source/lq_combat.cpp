#include "unit_test_lq_pch.h"

#include "static_lq\combat\combat.h"
#include "static_lq\combat\simple\simple_combat_output.h"
#include "static_lq\combat\simple\simple_combat_topology.h"
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
            std::shared_ptr<static_lq::ICombatOutput> combat_output = std::make_shared<static_lq::SimpleCombatOutput>([](const std::string& message)
            {
                Logger::WriteMessage(message.c_str());
            });
            std::shared_ptr<static_lq::RandomSequence> random_sequence = std::make_shared<static_lq::RandomSequence>(0);
            std::shared_ptr<static_lq::ICombatTopology> combatant_topology = std::make_shared<static_lq::SimpleCombatTopology>();

            static_lq::Combat combat(
                combat_output,
                random_sequence,
                combatant_topology
                );
		}

    };
}
