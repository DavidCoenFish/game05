#pragma once

class IDagThreadedValue;

class IDagThreadedNode
{
public:
	virtual ~IDagThreadedNode(){}
	virtual const std::string& GetName() const = 0;
	virtual void SetOutput(IDagThreadedNode* const in_node) = 0;
	virtual void RemoveOutput(IDagThreadedNode* const in_node) = 0;
	virtual std::shared_ptr<IDagThreadedValue> GetValue() = 0;
	virtual void MarkDirty() = 0;

	// move the following to only DagThreadedCalculate?
	//virtual void AddInputStack(IDagThreadedNode* const in_node) = 0;
	//virtual void RemoveInputStack(IDagThreadedNode* const in_node) = 0;
	//virtual void SetInputIndex(IDagThreadedNode* const in_node, const int in_index) = 0;


	//virtual const bool GetDirty() const = 0;

};
