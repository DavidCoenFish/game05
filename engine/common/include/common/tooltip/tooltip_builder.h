#pragma once

class ITooltip;
class LocaleSystem;
enum class LocaleISO_639_1;

/*
use cases, get tooltip from dag collection, for combatant display name, for combat log of operation (new turn, save throw, damage action)

//in_tooltip_locale_key_text_or_empty
_l.<key>		lookup locale system with key
_c.<ISO_639_1>		change the language code, <ISO_639_1> should be a LocaleISO_639_1 code ["en", "zh-cn", "uk",...]
				note. tooltip text will only have one locale code
_value			the value of the data source as a string
_value_bool		the value of the data source as a true/ false string
<x>				any other token is passed to the data source

//in_tooltip_locale_key_children_or_empty
_text			replace with the string that was generated for the in_tooltip_locale_key_text_or_empty
_v.<x>			verbosity, <x> -> any int for verbosity filtering tooltip text, -1 is development?
_l.<key>		lookup locale system with key
_c.<ISO_639_1>		change the language code, <ISO_639_1> should be a LocaleISO_639_1 code ["en", "zh-cn", "uk",...]
_value			the value of the data source as a string
_value_bool		the value of the data source as a true/ false string
_s_all_comma	list every tooltip of items in the stack input, comma seperated
_s.<x>			index to stack input tooltip
_i.<x>			index to index input tooltip
<x>				any other token is passed to the data source

want to allow a verbosity value for tooltip sequence
"Fin the human (average)" // v1: "human", v2: "human (average)", v3: "Fin the human (average)"
<"human", [ <"Fin the ", v=3>, <"human", v=1>, <" (average)", v=2>]>
"{species}" // in_tooltip_locale_key_text_or_empty
"{_v.3}{display_name} the {_v.1}{species}{_v.2} ({type})" // in_tooltip_locale_key_children_or_empty

"Health points ({_value})" // in_tooltip_locale_key_text_or_empty
"{_text} = {damage_tollerance} - {damage}" // in_tooltip_locale_key_children_or_empty
"Health points (7) = Damage Tollerance (15) - Damage (8)"
"Damage Tollerance (15) = 10 + 1d10 (5)" "1d10 (5) = seed (1387289) trace (7)"
"Damage (8) = Physical damage (6) + Paralisis damage (2)"

the "Health Points" dag nod returns a tooltip (TooltipLink)  with text "Health points (7)" and with an array of children

<"Health points (7),
	[
	"Health points (7) = ",
	<"Damage Tollerance (15)",["Damage Tollerance (15) = 10 + ", <"1d10 (5)", ["1d10 (5) = seed (1387289) trace (7)"]>]>,
	" - ",
	<"Damage (8)", ["Damage (8) = Physical damage (6) + Paralisis damage (2)"]>
	]
>

*/

class ITooltipBuilderDataSource
{
public:
	typedef std::variant<bool, int32_t, std::string> TValue;
	virtual const TValue GetValue() = 0;
	virtual const std::string GetText(
		const std::string& in_token, 
		const LocaleSystem& in_locale_system, 
		const LocaleISO_639_1 in_locale) = 0;
	/// ideally would just return a tooltip, but if we want to list all the items on a calculate node stack input, then pass the child array and current verbosity
	virtual void AppendChildren(
		std::vector<std::shared_ptr<ITooltip>>& in_out_children,
		const int in_clone_verbosity,
		const std::string& in_token, 
		const LocaleSystem& in_locale_system, 
		const LocaleISO_639_1 in_locale) = 0;

};

namespace TooltipBuilder
{
	void RegisterLocaleSystem(LocaleSystem& in_out_locale_system);
	const std::string GetLocaleKeyCommaSeperator();

	/// in_data_source is not const as dag collection can do calculations on get value 
	const std::shared_ptr<ITooltip> BuildTooltip(
		const std::string& in_tooltip_locale_key_text_or_empty,
		const std::string& in_tooltip_locale_key_children_or_empty,
		const LocaleSystem& in_locale_system, 
		const LocaleISO_639_1 in_locale,
		ITooltipBuilderDataSource& in_data_source
		);
}