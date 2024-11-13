#include "unit_test_pch.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_enum.h"
#include "common/locale/locale_string_format_map.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonLocaleSystem
{
	TEST_CLASS(GetValueFormatted)
	{
		public:
		TEST_METHOD(Simple)
		{
			LocaleSystem locale_system;
			locale_system.Append(LocaleISO_639_1::English, std::vector<LocaleSystem::Data>({{"key", "value {moo} {{}}"}}));
			LocaleStringFormatMap data_source(std::map<std::string, std::string>({{"moo", "bar"}}));

			locale_system.GetValueFormatted(LocaleISO_639_1::English, "key", data_source);
			const std::string result = data_source.GetResult();

			Assert::AreEqual(result.c_str(), "value bar {}");
		}

	};
}
