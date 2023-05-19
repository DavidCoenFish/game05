#pragma once

namespace Dag2
{
	enum DirtyCase
	{
		None = 0,
		ValueChanged = 1 << 0,
		NewValueNotZero = 1 << 1
	};

	template <typename TYPE>
	const bool IsZero(const TYPE& value)
	{
		return (0 == value);
	}

};
