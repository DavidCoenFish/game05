#pragma once

class IDagThreadedNode;
class IDagThreadedValue;

class IDagThreadedVisitor
{
public:
	virtual ~IDagThreadedVisitor(){};
	virtual const bool OnCalculate(
        const std::shared_ptr<IDagThreadedValue>& in_value,
	    const std::string& in_uid,
	    const std::string& in_display_name,
	    const std::string& in_display_description,
        const std::vector<std::string>& in_array_input_stack_uid,
        const std::vector<std::string>& in_array_input_index_uid,
        const std::vector<std::string>& in_array_output_uid
        //const std::vector<IDagThreadedNode*>& in_array_input_stack,
        //const std::vector<IDagThreadedNode*>& in_array_input_index,
        //const std::vector<IDagThreadedNode*>& in_array_output
        ) const = 0;

	virtual const bool OnVariable(
        const std::shared_ptr<IDagThreadedValue>& in_value,
	    const std::string& in_uid,
	    const std::string& in_display_name,
	    const std::string& in_display_description
        //const std::vector<IDagThreadedNode*>& in_array_input_stack,
        //const std::vector<IDagThreadedNode*>& in_array_input_index,
        //const std::vector<IDagThreadedNode*>& in_array_output
        ) const = 0;

};
