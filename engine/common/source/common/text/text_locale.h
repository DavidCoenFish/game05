#pragma once

#include <hb.h>

class TextLocale
{
public:
	TextLocale(
		const hb_direction_t in_direction,
		const hb_script_t in_script,
		//const std::string& in_language
		const hb_language_t in_language
		);

	hb_direction_t GetDirection() const { return _direction; }
	hb_script_t GetScript() const { return _script; }
	hb_language_t GetLanguage() const { return _language; }

private:
	hb_direction_t _direction;
	hb_script_t _script;
	//std::string _language;
	hb_language_t _language;

};
