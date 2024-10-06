#pragma once

#include "common/math/vector_float2.h"
#include "common/math/vector_float4.h"

class IUIComponent;
class UIData;
class UIComponentTooltipLayer;
class UIComponentComboBoxLayer;
class VectorInt2;
struct UIManagerDealInputParam;
class UIHierarchyNodeChild;

struct TooltipRequestData
{
	explicit TooltipRequestData(
		const VectorFloat2& in_touch_pos = VectorFloat2(),
		const VectorFloat4& in_emitter_screenspace = VectorFloat4(),
		const std::string& in_text_run_string = std::string(),
		void* const in_source_token = nullptr
		)
		: _touch_pos(in_touch_pos)
		, _emitter_screenspace(in_emitter_screenspace)
		, _text_run_string(in_text_run_string)
		, _source_token(in_source_token)
	{
		// Nop
	}
	VectorFloat2 _touch_pos;
	VectorFloat4 _emitter_screenspace;
	std::string _text_run_string;
	void* _source_token;
};

struct UIRootInputStateTouch
{
	explicit UIRootInputStateTouch(
		const VectorFloat2& in_touch_pos_current = VectorFloat2(),
		void* const in_active_source_token = nullptr,
		const int in_id = 0,
		const float in_active_duration = 0.0f,
		const bool in_active = false,
		const bool in_end = false
		);

	/// where is the touch currently, in screen space lower left [-1,-1 ... 1,1]
	VectorFloat2 _touch_pos_current;
	/// The source token of the component that the touch started over, or the first source token found while active, to allow click drag onto element?
	void* _active_source_token;
	/// The touch id
	int _id;
	/// Duration of the touch, vaild while active or end is true
	float _active_duration;
	/// Mouse button down, or touch active
	bool _active;
	/// did the touch start this frame
	//bool _start; use null _active_source_token to allow click drag off element
	/// did the touch end this frame
	bool _end;

	/// mark the touch as consumed if an element has used the touch
	bool _consumed;
};

class UIRootInputState
{
public:
	UIRootInputState();

	void Update(
		const UIManagerDealInputParam& in_param,
		const VectorInt2& in_root_size,
		UIData* const in_tooltip_layer_source_token,
		UIData* const in_combo_box_layer_source_token
		);

	std::vector<UIRootInputStateTouch>& GetTouchArray() { return _touch_array; }
	const float GetTimeDelta() const { return _time_delta; }

	void SubmitComponent(IUIComponent* const in_component);

	void RequestTooltip(
		const VectorFloat2& in_touch_pos, 
		const VectorFloat4& in_emitter_screenspace_clip, 
		const std::string& in_text_run_string,
		void* const in_source_token
		);

	/// Finialise tooltip, we pass the id of tooltip layer into our Update method
	void FinialiseTooltip();

	void RequestComboBoxDropdown(
		std::shared_ptr<UIHierarchyNodeChild>& in_dropdown
		);

	void FinialiseComboBox();

private:
	std::vector<UIRootInputStateTouch> _touch_array;

	/// current focus, using unique pointer so no weak pointer, so just traverse hierarchy to update flag?
	//void* _focus_source_token;
	float _time_delta;

	// cache the tooltip layer reference
	UIData* _tooltip_layer_source_token;
	UIComponentTooltipLayer* _tooltip_layer;
	std::vector<TooltipRequestData> _tooltip_request_data;

	// combo box dropdown request data
	std::shared_ptr<UIHierarchyNodeChild> _combo_box_dropdown;
	UIData* _combo_box_layer_source_token;
	UIComponentComboBoxLayer* _combo_box_layer;


};
