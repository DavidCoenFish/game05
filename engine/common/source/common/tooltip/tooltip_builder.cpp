#include "common/common_pch.h"
#include "common/tooltip/tooltip_builder.h"

#include "common/locale/locale_enum.h"
#include "common/locale/locale_system.h"
#include "common/locale/i_locale_string_format.h"
#include "common/tooltip/tooltip_text.h"
#include "common/tooltip/tooltip_link.h"

namespace
{
	constexpr char s_locale_key_true[] = "c_dag_true";
	constexpr char s_locale_key_false[] = "c_dag_false";

	const std::string DealValue(ITooltipBuilderDataSource& in_data_source, const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale)
	{
		const ITooltipBuilderDataSource::TValue value = in_data_source.GetValue();
		if (std::holds_alternative<bool>(value))
		{
			if (true == std::get<bool>(value))
			{
				return in_locale_system.GetValue(in_locale, s_locale_key_true);
			}
			else
			{
				return in_locale_system.GetValue(in_locale, s_locale_key_false);
			}
		}
		else if (std::holds_alternative<int32_t>(value))
		{
			return std::to_string(std::get<int32_t>(value));
		}
		else if (std::holds_alternative<std::string>(value))
		{
			return std::get<std::string>(value);
		}
		return std::string();
	}

	const std::string DealValueBoolean(ITooltipBuilderDataSource& in_data_source, const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale)
	{
		const ITooltipBuilderDataSource::TValue value = in_data_source.GetValue();
		bool value_bool = false;
		if (std::holds_alternative<bool>(value))
		{
			value_bool = std::get<bool>(value);
		}
		else if (std::holds_alternative<int32_t>(value))
		{
			value_bool = 0 != std::get<int32_t>(value);
		}

		return in_locale_system.GetValue(in_locale, value_bool ? s_locale_key_true : s_locale_key_false);
	}

	class TooltipTextBuilder : public ILocaleStringFormat
	{
	public:
		TooltipTextBuilder(
			std::string& out_text,
			LocaleISO_639_1& out_locale,
			ITooltipBuilderDataSource& in_data_source
			)
		: _text(out_text)
		, _locale(out_locale)
		, _data_source(in_data_source)
		{
			// nop
		}
	private:
		void AccumulateString(const std::string& in_value, const LocaleISO_639_1) override
		{
			_text += in_value;
		}

		void AccumulateToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int) override
		{
			// having some conceptual issues with having verbosity set in the text of the tooltip, more of a child thing
			if (in_token == "_value")
			{
				_text += DealValue(_data_source, in_locale_system, _locale);
			}
			else if (in_token == "_value_boolean")
			{
				_text += DealValueBoolean(_data_source, in_locale_system, _locale);
			}
			else if (std::string::npos != in_token.rfind("_c.", 0))
			{
				const std::string sub_string = in_token.substr(3);
				const LocaleISO_639_1 code = EnumSoftBind<LocaleISO_639_1>::EnumFromString(sub_string);
				_locale = (code == LocaleISO_639_1::Default) ? in_locale : code;
			}
			else if (std::string::npos != in_token.rfind("_l.", 0))
			{
				const std::string sub_string = in_token.substr(3);
				_text += in_locale_system.GetValue(_locale, sub_string);
			}
			else
			{
				const std::shared_ptr<ITooltip> tooltip = _data_source.GetTooltip(in_token, in_locale_system, _locale);
				if (nullptr != tooltip)
				{
					_text += tooltip->GetText();
				}
			}
		}

	private:
		std::string& _text;
		LocaleISO_639_1& _locale;
		ITooltipBuilderDataSource& _data_source;

	};

	class TooltipChildrenBuilder : public ILocaleStringFormat
	{
	public:
		TooltipChildrenBuilder(
			std::vector<std::shared_ptr<ITooltip>>& out_children,
			const std::string& in_text,
			const LocaleISO_639_1 in_locale,
			ITooltipBuilderDataSource& in_data_source
			)
			: _children(out_children)
			, _text(in_text)
			, _data_source(in_data_source)
			, _verbosity(0)
			, _locale(in_locale)
		{
			// nop
		}
	private:
		void AccumulateString(const std::string& in_value, const LocaleISO_639_1 in_locale) override
		{
			in_locale;
			_children.push_back(std::make_shared<TooltipText>(in_value, _locale, _verbosity));
		}
		void AccumulateToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, const std::string& in_token, const int) override
		{
			if (in_token == "_value")
			{
				_children.push_back(std::make_shared<TooltipText>(
					DealValue(_data_source, in_locale_system, _locale),
					_locale, _verbosity));
			}
			else if (in_token == "_value_boolean")
			{
				_children.push_back(std::make_shared<TooltipText>(
					DealValueBoolean(_data_source, in_locale_system, _locale),
					_locale, _verbosity));
			}
			else if (in_token == "_text")
			{
				_children.push_back(std::make_shared<TooltipText>(_text, _locale, _verbosity));
			}
			else if (std::string::npos != in_token.rfind("_c.", 0))
			{
				const std::string sub_string = in_token.substr(3);
				const LocaleISO_639_1 code = EnumSoftBind<LocaleISO_639_1>::EnumFromString(sub_string);
				_locale = (code == LocaleISO_639_1::Default) ? in_locale : code;
			}
			else if (std::string::npos != in_token.rfind("_l.", 0))
			{
				const std::string sub_string = in_token.substr(3);

				_children.push_back(std::make_shared<TooltipText>(
					in_locale_system.GetValue(_locale, sub_string),
					_locale, _verbosity));
			}
			else if (std::string::npos != in_token.rfind("_v.", 0))
			{
				const std::string sub_string = in_token.substr(3);
				_verbosity = std::stoi(sub_string);
			}
			else
			{
				const std::shared_ptr<ITooltip> tooltip = _data_source.GetTooltip(in_token, in_locale_system, in_locale);
				if (nullptr != tooltip)
				{
					_children.push_back(tooltip->Clone(_verbosity));
				}
			}
		}

	private:
		std::vector<std::shared_ptr<ITooltip>>& _children;
		const std::string _text = {};
		ITooltipBuilderDataSource& _data_source;
		int32_t _verbosity = 0;
		LocaleISO_639_1 _locale = LocaleISO_639_1::Default;
	};

}

void TooltipBuilder::RegisterLocaleSystem(LocaleSystem& in_out_locale_system)
{
	const std::vector<LocaleSystem::Data> data = {
		{s_locale_key_true, "True"},
		{s_locale_key_false, "False"},
		};
	in_out_locale_system.Append(LocaleISO_639_1::Default, data);
}

const std::shared_ptr<ITooltip> TooltipBuilder::BuildTooltip(
	const std::string& in_tooltip_locale_key_text_or_empty,
	const std::string& in_tooltip_locale_key_children_or_empty,
	const LocaleSystem& in_locale_system, 
	const LocaleISO_639_1 in_locale,
	ITooltipBuilderDataSource& in_data_source
	)
{
	if (true == in_tooltip_locale_key_text_or_empty.empty())
	{
		return nullptr;
	}

	std::string text = {};
	LocaleISO_639_1 locale = in_locale;
	TooltipTextBuilder tooltip_text_builder(text, locale, in_data_source);
	in_locale_system.GetValueFormatted(in_tooltip_locale_key_text_or_empty, tooltip_text_builder);
	if (true == text.empty())
	{
		return nullptr;
	}

	if (false == in_tooltip_locale_key_children_or_empty.empty())
	{
		std::vector<std::shared_ptr<ITooltip>> children = {};
		TooltipChildrenBuilder tooltip_children_builder(children, text, locale, in_data_source);
		in_locale_system.GetValueFormatted(in_tooltip_locale_key_children_or_empty, tooltip_children_builder);
		if (0 < children.size())
		{
			return std::make_shared<TooltipLink>(text, locale, 0, children);
		}
	}

	return std::make_shared<TooltipText>(text, locale, 0);

}
