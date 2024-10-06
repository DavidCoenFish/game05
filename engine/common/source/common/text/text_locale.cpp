#include "common/common_pch.h"
#include "common/text/text_locale.h"

TextLocale::TextLocale(
	const hb_direction_t in_direction,
	const hb_script_t in_script,
	//const std::string& in_language
	const hb_language_t in_language
	)
	: _direction(in_direction)
	, _script(in_script)
	, _language(in_language)
{
	// Nop
}
