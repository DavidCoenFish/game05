#pragma once

#include "common/ui/ui_data/ui_data.h"
#include "common/locale/locale_enum.h"
#include "common/text/text_enum.h"

class LocaleSystem;
struct UIHierarchyNodeUpdateParam;

/// String intentially doesn't have any markup like font, size, colour. to customise that see "text run"
class UIDataString : public UIData
{
	typedef UIData TSuper;
public:
	/// let the text manager what locale hint to render text as, for harfbuz
	UIDataString(
#ifdef _DEBUG
		const std::string& in_debug_name,
#endif
		const UILayout& in_layout,
		const UIBaseColour& in_base_colour,
		const UITintColour& in_tint_colour,
		const std::vector<std::shared_ptr<UIEffectData>>& in_array_effect_data,
		const std::string& in_data = std::string(),
		const LocaleISO_639_1 in_locale = LocaleISO_639_1::Default,
		const bool in_width_limit_enabled = false,
		const TextEnum::HorizontalLineAlignment in_horizontal = TextEnum::HorizontalLineAlignment::Left,
		const TextEnum::VerticalBlockAlignment in_vertical = TextEnum::VerticalBlockAlignment::Bottom
		);
	virtual ~UIDataString();

	// Todo: change to visitor to future proof threading?
	//const std::string& GetStringRef() const { return _data; }
	//const LocaleISO_639_1 GetLocale() const { return _locale; }
	void SetString(const std::string& in_data);
	void SetLocale(const LocaleISO_639_1 in_locale);

private:
	virtual const bool ApplyComponent(
		std::unique_ptr<IUIComponent>& in_out_component,
		const UIHierarchyNodeUpdateParam& in_param
		) override;

private:
	std::string _data;
	LocaleISO_639_1 _locale;
	bool _width_limit_enabled;
	TextEnum::HorizontalLineAlignment _horizontal;
	TextEnum::VerticalBlockAlignment _vertical;

};
