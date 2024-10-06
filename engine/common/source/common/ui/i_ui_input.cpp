#include "common/common_pch.h"
#include "common/ui/i_ui_input.h"

IUIInput::~IUIInput()
{
	// Nop
}

void IUIInput::OnInputTouch(
	const VectorFloat4&,
	const VectorFloat2&,
	std::string&
	)
{
	// Nop
}

void IUIInput::OnInputClick(
	UIRootInputState&,
	const VectorFloat4&,
	const VectorFloat2&
	)
{
	// Nop
}

void IUIInput::OnInputRepeat(
	const VectorFloat4&,
	const VectorFloat2&,
	const float,
	const float,
	std::string&
	)
{
	// Nop
}

void IUIInput::OnHover(
	const VectorFloat4&,
	const VectorFloat2&,
	std::string&
	)
{
	// Nop
}


