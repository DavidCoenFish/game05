#pragma once

class DagThreadedCollection;
class IDagThreadedValue;
class IDagThreadedVisitor;
class LocaleSystem;
class ITooltip;
enum class LocaleISO_639_1;

class IDagThreadedNode
{
public:
	virtual ~IDagThreadedNode(){}
	virtual const std::string& GetUid() const = 0;

	/// maintain an internal link to the output node to allow marking downstream dirty on change
	virtual void SetOutput(IDagThreadedNode* const in_node) = 0;
	virtual void RemoveOutput(IDagThreadedNode* const in_node) = 0;

	/// allow an upstream node to indicate that a input value has changes and we need to mark our result as dirty/ in need of calculation
	virtual void MarkDirty() = 0;

	/// get the value the node holds, non const as it is calculated/generated if dirty. 
	virtual std::shared_ptr<IDagThreadedValue> GetValue() = 0;

	/// get the tooltip for the given locale, non const as it is re-generated if dirty/ different locale
	virtual std::shared_ptr<ITooltip> GetTooltip(const DagThreadedCollection& in_collection, const LocaleSystem& in_locale_system, const LocaleISO_639_1 in_locale) = 0;

	/// return true to continue visiting
	virtual const bool Visit(IDagThreadedVisitor& visitor) = 0;

	/// display name could be locale key or the name itself?
	//virtual const std::string& GetDisplayName() const = 0;
	//virtual const std::string& GetTooltipRaw() const = 0;

};
