#include "unit_test_lq_pch.h"
#include "static_lq\bestiary\bestiary.h"

#include "common\locale\locale_system.h"
#include "static_lq\name\name_system.h"
#include "static_lq\name\name_system_generator_random.h"
#include "static_lq\combat\i_combatant.h"
#include "static_lq\combat\combat_enum.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LqBestiary
{
	TEST_CLASS(StaticMethods)
	{
	public:
		TEST_METHOD(FactoryDefaultGiantSpider)
		{
			std::shared_ptr<LocaleSystem> locale_system = std::make_shared<LocaleSystem>();
			std::shared_ptr<StaticLq::NameSystem> name_system = std::make_shared<StaticLq::NameSystem>();
			name_system->AddGenerator(StaticLq::NameSystem::GetKeyGiantSpider(), StaticLq::NameSystemGeneratorRandom::FactoryGeneratorGiantSpider());

			auto giant_spider = StaticLq::Bestiary::FactoryDefaultGiantSpider(*name_system, *locale_system);
			Assert::IsNotNull(giant_spider.get());
			Assert::AreEqual(1, giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::TCanContinueCombat));

			giant_spider->ApplyDamageDelta(100, 200, 300);
			Assert::AreEqual(0, giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::TCanContinueCombat));
		}

		TEST_METHOD(FactoryDefaultGiantAnt)
		{
			std::shared_ptr<LocaleSystem> locale_system = std::make_shared<LocaleSystem>();
			std::shared_ptr<StaticLq::NameSystem> name_system = std::make_shared<StaticLq::NameSystem>();
			name_system->AddGenerator(StaticLq::NameSystem::GetKeyGiantAnt(), StaticLq::NameSystemGeneratorRandom::FactoryGeneratorGiantAnt());

			auto item = StaticLq::Bestiary::FactoryDefaultGiantAnt(*name_system, *locale_system);
			Assert::IsNotNull(item.get());
		}

	};
}
