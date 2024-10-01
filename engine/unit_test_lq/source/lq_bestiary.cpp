#include "unit_test_lq_pch.h"
#include "static_lq\bestiary\bestiary.h"

#include "common\locale\locale_system.h"
#include "static_lq\name\name_system.h"
#include "static_lq\name\name_system_generator_random.h"
#include "static_lq\combat\i_combatant.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LqBestiary
{
	TEST_CLASS(StaticMethods)
	{
	public:
		TEST_METHOD(FactoryDefaultGiantSpider)
		{
            std::shared_ptr<LocaleSystem> locale_system = std::make_shared<LocaleSystem>();
            std::shared_ptr<static_lq::NameSystem> name_system = std::make_shared<static_lq::NameSystem>();
            name_system->AddGenerator(static_lq::NameSystem::GetKeyGiantSpider(), static_lq::NameSystemGeneratorRandom::FactoryGeneratorGiantSpider());

            auto giant_spider = static_lq::Bestiary::FactoryDefaultGiantSpider(*name_system, *locale_system);
			Assert::IsNotNull(giant_spider.get());
			Assert::AreEqual(true, giant_spider->IsAbleToContinueCombat());

            giant_spider->ApplyDamage(100, 200, 300);
			Assert::AreEqual(false, giant_spider->IsAbleToContinueCombat());
		}

		TEST_METHOD(FactoryDefaultGiantAnt)
		{
            std::shared_ptr<LocaleSystem> locale_system = std::make_shared<LocaleSystem>();
            std::shared_ptr<static_lq::NameSystem> name_system = std::make_shared<static_lq::NameSystem>();
            name_system->AddGenerator(static_lq::NameSystem::GetKeyGiantAnt(), static_lq::NameSystemGeneratorRandom::FactoryGeneratorGiantAnt());

            auto item = static_lq::Bestiary::FactoryDefaultGiantAnt(*name_system, *locale_system);
			Assert::IsNotNull(item.get());
		}

	};
}
