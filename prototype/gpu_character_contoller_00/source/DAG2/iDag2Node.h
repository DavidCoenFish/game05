#pragma once

class iDag2Value;

class iDag2Node
{
public:
	virtual void SetOutput(iDag2Node* const pNode) = 0;
	virtual void RemoveOutput(iDag2Node* const pNode) = 0;

	virtual void AddInputStack(iDag2Node* const pNode) = 0;
	virtual void RemoveInputStack(iDag2Node* const pNode) = 0;
	virtual void SetInputIndex(iDag2Node* const pNode, const int index) = 0;

	virtual iDag2Value* GetValue() = 0;

	virtual void MarkDirty() = 0;
	
	//virtual const bool GetDirty() const = 0;

	virtual void Unlink() = 0;

};
