#include "unit_test_lq_pch.h"
#include "static_lq\bestiary\bestiary.h"

#include "common\locale\locale_system.h"
#include "common\locale\locale_enum.h"
#include "static_lq\bestiary\bestiary_enum.h"
#include "static_lq\bestiary\bestiary_pool.h"
#include "static_lq\combat\i_combatant.h"
#include "static_lq\combat\combat_enum.h"
#include "static_lq\name\name_system.h"
#include "static_lq\name\name_system_generator_random.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LqBestiary
{
	TEST_CLASS(StaticMethods)
	{
	public:
		TEST_METHOD(FactoryTestDataGiantAnt)
		{
			std::shared_ptr<LocaleSystem> locale_system = std::make_shared<LocaleSystem>();
			Assert::IsNotNull(locale_system.get());

			std::shared_ptr<StaticLq::NameSystem> name_system = std::make_shared<StaticLq::NameSystem>();
			name_system->AddGenerator(StaticLq::NameSystem::GetKeyGiantAnt(), StaticLq::NameSystemGeneratorRandom::FactoryGeneratorGiantAnt());
			
			locale_system->SetLocaleAndPopulate(LocaleISO_639_1::Default);

			auto bestiary_pool = StaticLq::BestiaryPool::FactoryWithTestData();
			Assert::IsNotNull(bestiary_pool.get());

			auto giant_ant = bestiary_pool->MakeCombatant(
				StaticLq::BestiaryPool::GetTestDataKeyGiantAnt(),
				*name_system,
				*locale_system
				);
			Assert::IsNotNull(giant_ant.get());

			const std::string display_name = locale_system->GetValue(giant_ant->GetDisplayName());
			Logger::WriteMessage(display_name.c_str());
		}
	};

/*
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
			Logger::WriteMessage((std::to_string(giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::TDamageTollerance)) + " Damage tollerance\n").c_str());
			Logger::WriteMessage((std::to_string(giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::THealthPoints)) + " Health Points\n").c_str());
			Assert::AreEqual(1, giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::TCanContinueCombat));

			giant_spider->ApplyDamageDelta(100, 200, 300);
			Logger::WriteMessage((std::to_string(giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::TDamageTollerance)) + " Damage tollerance\n").c_str());
			Logger::WriteMessage((std::to_string(giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::THealthPoints)) + " Health Points\n").c_str());
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
*/
}
