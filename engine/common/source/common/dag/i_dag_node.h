#pragma once

class IDagValue;
class Tooltip;

class IDagNode
{
public:
	virtual ~IDagNode(){}
	virtual const std::string& GetName() const = 0;
    //virtual std::mutex& GetOutputMutex() = 0;
	virtual void SetOutput(IDagNode* const in_node) = 0;
	virtual void RemoveOutput(IDagNode* const in_node) = 0;
	virtual std::shared_ptr<IDagValue> GetValue() = 0;
    virtual std::shared_ptr<Tooltip> MakeTooltip() = 0;

	virtual void MarkDirty() = 0;

};
