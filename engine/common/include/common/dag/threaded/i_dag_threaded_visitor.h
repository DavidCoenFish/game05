#pragma once

class IDagThreadedNode;
class IDagThreadedValue;

class IDagThreadedVisitor
{
public:
	typedef IDagThreadedNode* NodeID;

	virtual ~IDagThreadedVisitor(){};
	/// trying to avoid IDagThreadedNode being exposed to the public
	virtual const bool OnCalculate(
		const std::shared_ptr<IDagThreadedValue>& in_value,
		const int in_calculate_id,
		const std::string& in_uid,
		const std::string& in_tooltip_locale_key_text,
		const std::string& in_tooltip_locale_key_children,
		const std::vector<NodeID>& in_array_input_stack,
		const std::vector<NodeID>& in_array_input_index,
		const std::vector<NodeID>& in_array_output
		) = 0;

	virtual const bool OnVariable(
		const std::shared_ptr<IDagThreadedValue>& in_value,
		const std::string& in_uid,
		const std::string& in_tooltip_locale_key_text,
		const std::string& in_tooltip_locale_key_children
		) = 0;

};
