#include "CommonPCH.h"

#include "Common/PathFinder/PathFinderLocation.h"

namespace
{
	union DataPacker
	{
		short s[2];
		int i;
	};
};

const PathFinder::Location PathFinder::PackLocation(const short x, const short y)
{
	DataPacker dataPacker;
	dataPacker.s[0] = x;
	dataPacker.s[1] = y;
	return dataPacker.i;
}

void PathFinder::UnPackLocation(PASS_LOCATION_CONST input, short& x, short& y)
{
	DataPacker dataPacker;
	dataPacker.i = input;
	x = dataPacker.s[0];
	y = dataPacker.s[1];
	return;
}

void PathFinder::UnPackLocationMinus(PASS_LOCATION_CONST input, PASS_LOCATION_CONST delta, short& x, short& y)
{
	DataPacker dataPackerInput;
	DataPacker dataPackerDelta;
	dataPackerInput.i = input;
	dataPackerDelta.i = delta;
	x = dataPackerInput.s[0] - dataPackerDelta.s[0];
	y = dataPackerInput.s[1] - dataPackerDelta.s[1];
	return;
}

