#pragma once

class iDag2Node;

class Dag2Collection
{
public:
	Dag2Collection();
	~Dag2Collection();

	static const std::string MakeUniqueName();

	void AddNode(const std::string& name, const std::shared_ptr< iDag2Node >& in_dagNode);
	void RemoveNode(const std::string& name);

	std::shared_ptr< iDag2Node > GetNode(const std::string& name) const;

	static void AddLinkStack(const std::shared_ptr< iDag2Node >& pInput, const std::shared_ptr< iDag2Node >& pOutput);
	static void RemoveLinkStack(const std::shared_ptr< iDag2Node >& pInput, const std::shared_ptr< iDag2Node >& pOutput);
	static void SetLinkIndex(const std::shared_ptr< iDag2Node >& pInputOrNullptr, const int inputIndex, const std::shared_ptr< iDag2Node >& pOutput);

private:
	std::map< std::string, std::shared_ptr< iDag2Node > > m_dagNodeMap;

};
