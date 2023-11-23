#pragma once

#include "Common/PathFinder/PathFinderLocation.h"

namespace PathFinder
{
	template <int WIDTH, int HEIGHT>
	class RenderTile
	{
	public:
		typedef std::pair<int, float> TColorIndexHeight;
		typedef std::array<TColorIndexHeight, (WIDTH * HEIGHT)> TData;
		RenderTile(const Location& offset, const TData& data);

		void QueueModification(const Location& location, const int colorIndex);
		void ProcessModification();

	private:
		TData m_data;
		bool m_dirty;

	};
};