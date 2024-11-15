#include "static_lq/static_lq_pch.h"
#include "static_lq/name/name_system.h"

#include "common/locale/locale_system.h"
#include "common/locale/i_locale_data_provider.h"
#include "static_lq/name/i_name_system_generator.h"

namespace
{
	class LocaleDataProvider : public ILocaleDataProvider
	{
	public:
		LocaleDataProvider(const std::shared_ptr<StaticLq::NameSystem>& in_name_system)
		: _name_system(in_name_system)
		{
			// _nop
		}
	private:
		void Populate(LocaleSystem& in_out_locale_system, const LocaleISO_639_1 in_locale) const override
		{
			std::shared_ptr<StaticLq::NameSystem> name_system = _name_system.lock();
			if (name_system)
			{
				name_system->AddAllValuesToLocaleSystem(in_out_locale_system, in_locale);
			}
		}

	private:
		std::weak_ptr<StaticLq::NameSystem> _name_system;
	};
}

class NameSystemImplementation
{
public:
	NameSystemImplementation()
	{
		// nop
	}
	~NameSystemImplementation()
	{
		// nop
	}

	void AddGenerator(const std::string& in_key, const std::shared_ptr<StaticLq::INameSystemGenerator>& in_generator)
	{
		_generator_map[in_key] = in_generator;
	}

	void AddAllValuesToLocaleSystem(LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale)
	{
		std::vector<LocaleSystem::Data> data;
		for (const auto& item : _name_instance_array)
		{
			const auto& found = _generator_map.find(item.key);
			if ((found == _generator_map.end()) || (nullptr == found->second))
			{
				continue;
			}

			const std::string key = StaticLq::INameSystemGenerator::GenerateKey(item.key, item.seed);
			const std::string value = found->second->GenerateName(item.seed, in_locale);

			data.push_back({key, value});
		}

		in_locale_system.Append(in_locale, data);
	}

	// return locale key, add locale value for key to locale system for the current locale, keep record of used generation data to regenerate for locale system on locale change via provider
	const std::string GenerateName(const std::string& in_key, const int in_seed, LocaleSystem& in_locale_system)
	{
		const auto& found = _generator_map.find(in_key);
		if (found == _generator_map.end())
		{
			return "";
		}

		const std::string key = found->second->GenerateKey(in_key, in_seed);
		const std::string value = found->second->GenerateName(in_seed, in_locale_system.GetLocale());

		in_locale_system.AppendCurrent(key, value);

		return key;
	}

private:
	std::map<std::string, std::shared_ptr<StaticLq::INameSystemGenerator>> _generator_map;
	struct NameInstance
	{
		std::string key; 
		int seed;
	};
	/// track the instanced names so the locale provider
	std::vector<NameInstance> _name_instance_array;

};

const std::string StaticLq::NameSystem::GetKeySide()
{
	return "builtin_side";
}

const std::string StaticLq::NameSystem::GetKeyGiantAnt()
{
	return "builtin_giant_ant";
}

const std::string StaticLq::NameSystem::GetKeyGiantSpider()
{
	return "builtin_giant_spider";
}

void StaticLq::NameSystem::RegisterLocaleSystem(const std::shared_ptr<NameSystem>& in_name_system,  LocaleSystem& in_locale_system)
{
	in_locale_system.RegisterProvider(std::make_shared<LocaleDataProvider>(in_name_system));
}

StaticLq::NameSystem::NameSystem()
{
	_implementation = std::make_unique<NameSystemImplementation>();
}

StaticLq::NameSystem::~NameSystem()
{
	// nop
}

void StaticLq::NameSystem::AddGenerator(const std::string& in_key, const std::shared_ptr<INameSystemGenerator>& in_generator)
{
	_implementation->AddGenerator(in_key, in_generator);
}

const std::string StaticLq::NameSystem::GenerateName(const std::string& in_key, const int in_seed, LocaleSystem& in_locale_system)
{
	return _implementation->GenerateName(in_key, in_seed, in_locale_system);
}

void StaticLq::NameSystem::AddAllValuesToLocaleSystem(LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale)
{
	return _implementation->AddAllValuesToLocaleSystem(in_locale_system, in_locale);
}
