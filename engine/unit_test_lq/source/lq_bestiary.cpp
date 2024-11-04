#include "unit_test_lq_pch.h"
#include "static_lq\bestiary\bestiary.h"

#include "common\locale\locale_system.h"
#include "common\locale\locale_enum.h"
#include "common\tooltip\i_tooltip.h"
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

			auto self_tooltip = giant_ant->GetSelfTooltip(*locale_system, locale_system->GetLocale());
			std::string display_name = {};
			Assert::IsNotNull(self_tooltip.get());
			self_tooltip->Visit([&display_name](const std::string& in_text, const LocaleISO_639_1, const int32_t, const int32_t)->const bool{
				display_name = in_text;
				return false;
			});

			Logger::WriteMessage(display_name.c_str());
		}

		TEST_METHOD(FactoryDefaultGiantSpider)
		{
			std::shared_ptr<LocaleSystem> locale_system = std::make_shared<LocaleSystem>();
			Assert::IsNotNull(locale_system.get());

			std::shared_ptr<StaticLq::NameSystem> name_system = std::make_shared<StaticLq::NameSystem>();
			StaticLq::NameSystem::RegisterLocaleSystem(name_system, *locale_system);
			name_system->AddGenerator(StaticLq::NameSystem::GetKeyGiantSpider(), StaticLq::NameSystemGeneratorRandom::FactoryGeneratorGiantSpider());
			
			locale_system->SetLocaleAndPopulate(LocaleISO_639_1::Default);

			auto bestiary_pool = StaticLq::BestiaryPool::FactoryWithTestData();
			Assert::IsNotNull(bestiary_pool.get());


			auto giant_spider = bestiary_pool->MakeCombatant(
				StaticLq::BestiaryPool::GetTestDataKeyGiantSpider(),
				*name_system,
				*locale_system
				);
			Assert::IsNotNull(giant_spider.get());

			Logger::WriteMessage((std::to_string(giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::TDamageTollerance)) + " Damage tollerance\n").c_str());
			Logger::WriteMessage((std::to_string(giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::THealthPoints)) + " Health Points\n").c_str());
			Assert::AreEqual(1, giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::TCanContinueCombat));

			giant_spider->ApplyDamageDelta(100, 200, 300);
			Logger::WriteMessage((std::to_string(giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::TDamageTollerance)) + " Damage tollerance\n").c_str());
			Logger::WriteMessage((std::to_string(giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::THealthPoints)) + " Health Points\n").c_str());
			Assert::AreEqual(0, giant_spider->GetValue(StaticLq::CombatEnum::CombatantValue::TCanContinueCombat));
		}

	};
}
