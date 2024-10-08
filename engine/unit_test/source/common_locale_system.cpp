#include "unit_test_pch.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_enum.h"
#include "common/locale/locale_string_format_simple.h"
#include "common/locale/locale_string_format_lambda.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonLocaleSystem
{
	TEST_CLASS(GetValueFormatted)
	{
		public:
		TEST_METHOD(Simple)
		{
			LocaleSystem locale_system;
			locale_system.Append(LocaleISO_639_1::English, std::vector<LocaleSystem::Data>({{"key", "value {}"}}));
			LocaleStringFormatSimple data_source(std::vector<std::string>({"moo"}));

			locale_system.GetValueFormatted(LocaleISO_639_1::English, "key", data_source);
			const std::string result = data_source.GetResult();

			Assert::AreEqual(result.c_str(), "value moo");
		}

		TEST_METHOD(Escape)
		{
			LocaleSystem locale_system;
			locale_system.Append(LocaleISO_639_1::English, std::vector<LocaleSystem::Data>({{"key", "value {} {{}}"}}));
			LocaleStringFormatSimple data_source(std::vector<std::string>({"moo"}));

			locale_system.GetValueFormatted(LocaleISO_639_1::English, "key", data_source);
			const std::string result = data_source.GetResult();

			Assert::AreEqual(result.c_str(), "value moo {}");
		}

		TEST_METHOD(CheckTokens)
		{
			LocaleSystem locale_system;
			locale_system.Append(LocaleISO_639_1::English, std::vector<LocaleSystem::Data>({{"key", "value {one} {two} {three}"}}));
			LocaleStringFormatLambda data_source([](const LocaleISO_639_1 in_locale, const std::string& in_token, const int in_index)-> const std::string{
				Assert::AreEqual<int>(static_cast<int>(in_locale), static_cast<int>(LocaleISO_639_1::English));
				if (0 ==  in_index)
				{
					Assert::AreEqual(in_token.c_str(), "one");
					return "1";
				}
				else if (1 ==  in_index)
				{
					Assert::AreEqual(in_token.c_str(), "two");
					return "2";
				}
				else if (2 ==  in_index)
				{
					Assert::AreEqual(in_token.c_str(), "three");
					return "3";
				}
				else
				{
					Assert::Fail();
				}
				return std::string();
			});

			locale_system.GetValueFormatted(LocaleISO_639_1::English, "key", data_source);
			const std::string result = data_source.GetResult();

			Assert::AreEqual(result.c_str(), "value 1 2 3");
		}


	};
}
