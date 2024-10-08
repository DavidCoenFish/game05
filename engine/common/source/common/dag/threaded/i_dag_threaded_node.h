#pragma once

class DagThreadedCollection;
class IDagThreadedValue;
class IDagThreadedVisitor;
class LocaleSystem;
class Tooltip;

class IDagThreadedNode
{
public:
	virtual ~IDagThreadedNode(){}
	virtual const std::string& GetUid() const = 0;
	/// display name could be locale key or the name itself?
	virtual const std::string& GetDisplayName() const = 0;
	virtual const std::string& GetTooltipRaw() const = 0;
	virtual void SetOutput(IDagThreadedNode* const in_node) = 0;
	virtual void RemoveOutput(IDagThreadedNode* const in_node) = 0;
	virtual std::shared_ptr<IDagThreadedValue> GetValue() = 0;
	virtual std::shared_ptr<Tooltip> GetTooltip(const DagThreadedCollection& in_collection, const LocaleSystem& in_locale_system) = 0;
	/// return true to continue visiting 
	virtual const bool Visit(IDagThreadedVisitor& visitor) = 0;

	virtual void MarkDirty() = 0;

};
