#include "common/common_pch.h"
#include "common/locale/locale_system.h"
#include "common/locale/locale_enum.h"
#include "common/locale/i_locale_data_provider.h"
#include "common/locale/i_locale_string_format.h"

namespace
{
enum class State
{
	TDefault,
	TBracketStart,
	TBracketEnd,
	TToken,
};

void DealToken(const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale, std::string& in_out_token, int& in_out_index,
		ILocaleStringFormat& in_locale_string_format, std::string& in_out_result)
{
	// consume the result string if not empty
	if (false == in_out_result.empty())
	{
		in_locale_string_format.AccumulateString(in_out_result);
		in_out_result = {};
	}

	in_locale_string_format.AccumulateToken(in_locale_system, in_locale, in_out_token, in_out_index);
	in_out_token = {};
	in_out_index += 1;
	return;
}

};

class LocaleSystemImplementation
{
private:
	typedef std::map<std::string, std::string> TDataMap;
public:
	LocaleSystemImplementation()
	: _locale(LocaleISO_639_1::Invalid)
	, _cached_data_map(nullptr)
	{
		// Nop
	}

	~LocaleSystemImplementation()
	{
		// Nop
	}

	const std::string GetValue(
		const LocaleISO_639_1 in_locale,
		const std::string& in_key
		) const
	{
		TDataMap* const data_map = ReturnDataMap(in_locale);
		if (nullptr != data_map)
		{
			auto found = data_map->find(in_key);
			if (found != data_map->end())
			{
				return found->second;
			}
		}
		return std::string();
	}

	const std::string GetValue(
		const std::string& in_key
		) const
	{
		return GetValue(_locale, in_key);
	}

	void GetValueFormatted(
		const LocaleSystem& in_locale_system,
		const LocaleISO_639_1 in_locale,
		const std::string& in_key,
		ILocaleStringFormat& in_locale_string_format
		) const
	{
		const std::string value = GetValue(in_locale, in_key);
		std::string result;
		std::string token;
		State state = State::TDefault;
		int index = 0;

		for (const auto character : value)
		{
			if (character == '{')
			{
				if (state == State::TBracketStart) // escape
				{
					result += character;
					state = State::TDefault;
				}
				else if (state == State::TBracketEnd)
				{
					DealToken(in_locale_system, in_locale, token, index, in_locale_string_format, result);
					state = State::TDefault;
				}
				else if (state == State::TToken) // inside a token
				{
					token += character;
				}
				else 
				{
					state = State::TBracketStart;
				}
			}
			else if (character == '}')
			{
				if (state == State::TBracketStart)
				{
					DealToken(in_locale_system, in_locale, token, index, in_locale_string_format, result);
					state = State::TDefault;
				}
				if (state == State::TBracketEnd) // escape
				{
					result += character;
					state = State::TDefault;
				}
				else if (state == State::TToken) // end of markup token
				{
					DealToken(in_locale_system, in_locale, token, index, in_locale_string_format, result);
					state = State::TDefault;
				}
				else // start of escape?
				{
					state = State::TBracketEnd;
				}
			}
			else
			{
				if (state == State::TBracketStart)
				{
					state = State::TToken;
					token += character;
				}
				else if (state == State::TBracketEnd)
				{
					DealToken(in_locale_system, in_locale, token, index, in_locale_string_format, result);
					result += character;
					state = State::TDefault;
				}
				else if (state == State::TToken)
				{
					token += character;
				}
				else
				{
					result += character;
				}
			}
		}

		// did the input end on a bracket end
		if (state == State::TBracketEnd)
		{
			DealToken(in_locale_system, in_locale, token, index, in_locale_string_format, result);
		}

		if (false == result.empty())
		{
			in_locale_string_format.AccumulateString(result);
		}

		return;
	}

	void GetValueFormatted(
		const LocaleSystem& in_locale_system,
		const std::string& in_key,
		ILocaleStringFormat& in_locale_string_format
		) const
	{
		GetValueFormatted(in_locale_system, _locale, in_key, in_locale_string_format);
	}

	void RegisterProvider(const std::shared_ptr<ILocaleDataProvider>& in_provider)
	{
		DSC_ASSERT(nullptr != in_provider, "invlid param");
		_provider_array.push_back(in_provider);
	}

	void SetLocaleAndPopulate(LocaleSystem& that, const LocaleISO_639_1 in_locale)
	{
		if (_locale == in_locale)
		{
			return;
		}
		_cached_data_map = FindCreateDataMap(in_locale);
		_locale = in_locale;
		for (const auto& item : _provider_array)
		{
			item->Populate(that, _locale);
		}
	}

	const LocaleISO_639_1 GetLocale() const
	{
		return _locale;
	}

	const std::vector<LocaleISO_639_1> GatherLocale() const
	{
		std::vector<LocaleISO_639_1> result;
		for (auto iter : _locale_data_map)
		{
			result.push_back(iter.first);
		}
		return result;
	}

	void Append(
		const LocaleISO_639_1 in_locale,
		const std::vector<LocaleSystem::Data>& in_data
		)
	{
		TDataMap* const data_map = FindCreateDataMap(in_locale);
		DSC_ASSERT(nullptr != data_map, "invalid case");

		for (const auto iter : in_data)
		{
			(*data_map)[iter._key] = iter._value;
		}
		return;
	}

	void AppendCurrent(
		const std::string& in_key,
		const std::string& in_value
		)
	{
		TDataMap* const data_map = FindCreateDataMap(_locale);
		DSC_ASSERT(nullptr != data_map, "invalid case");

		(*data_map)[in_key] = in_value;
	}

private:
	TDataMap* const FindCreateDataMap(
		const LocaleISO_639_1 in_locale
		)
	{
		TDataMap* data_map = nullptr;
		auto found = _locale_data_map.find(in_locale);
		if (found == _locale_data_map.end())
		{
			auto shared_data_map = std::make_shared<TDataMap>();
			_locale_data_map[in_locale] = shared_data_map;
			data_map = shared_data_map.get();
		}
		else
		{
			data_map = found->second.get();
		}

		return data_map;
	}

	TDataMap* const ReturnDataMap(
		const LocaleISO_639_1 in_locale
		) const
	{
		if (in_locale == _locale)
		{
			return _cached_data_map;
		}

		const auto& found = _locale_data_map.find(in_locale);
		if (found != _locale_data_map.end())
		{
			return found->second.get();
		}
		return nullptr;
	}

private:
	LocaleISO_639_1 _locale;
	/// most of the time we will just being using the selected locale, so save a lookup
	TDataMap* _cached_data_map;
	std::map<LocaleISO_639_1, std::shared_ptr<TDataMap>> _locale_data_map;
	std::vector<std::shared_ptr<ILocaleDataProvider>> _provider_array;
};

LocaleSystem::LocaleSystem()
{
	_implementation = std::make_unique<LocaleSystemImplementation>();
}

LocaleSystem::~LocaleSystem()
{
	// Nop
}

const std::string LocaleSystem::GetValue(
	const LocaleISO_639_1 in_locale,
	const std::string& in_key
	) const
{
	return _implementation->GetValue(in_locale, in_key);
}

const std::string LocaleSystem::GetValue(
	const std::string& in_key
	) const
{
	return _implementation->GetValue(in_key);
}

void LocaleSystem::GetValueFormatted(
	const LocaleISO_639_1 in_locale,
	const std::string& in_key,
	ILocaleStringFormat& in_locale_string_format
	) const
{
	_implementation->GetValueFormatted(*this, in_locale, in_key, in_locale_string_format);
	return;
}

void LocaleSystem::GetValueFormatted(
	const std::string& in_key,
	ILocaleStringFormat& in_locale_string_format
	) const
{
	_implementation->GetValueFormatted(*this, in_key, in_locale_string_format);
	return;
}

void LocaleSystem::RegisterProvider(const std::shared_ptr<ILocaleDataProvider>& in_provider)
{
	_implementation->RegisterProvider(in_provider);
	return;
}

void LocaleSystem::SetLocaleAndPopulate(const LocaleISO_639_1 in_locale)
{
	_implementation->SetLocaleAndPopulate(*this, in_locale);
	return;
}

const LocaleISO_639_1 LocaleSystem::GetLocale() const
{
	return _implementation->GetLocale();
}

const std::vector<LocaleISO_639_1> LocaleSystem::GatherLocale() const
{
	return _implementation->GatherLocale();
}

void LocaleSystem::Append(
	const LocaleISO_639_1 in_locale,
	const std::vector<Data>& in_data
	)
{
	_implementation->Append(in_locale, in_data);
	return;
}

void LocaleSystem::AppendCurrent(
	const std::string& in_key,
	const std::string& in_value
	)
{
	_implementation->AppendCurrent(in_key, in_value);
	return;
}

