#pragma once

//is our implementation of Location a typedefed plain old data, or an object (or union, class, std::pair...)
#define PASS_LOCATION_CONST const Location
//#define PASS_LOCATION_CONST const Location&

namespace PathFinder
{
	typedef int Location;
	const Location PackLocation(const short x, const short y);
	void UnPackLocation(PASS_LOCATION_CONST input, short& x, short& y);
	void UnPackLocationMinus(PASS_LOCATION_CONST input, PASS_LOCATION_CONST delta, short& x, short& y);

	//class Location
	//{
	//public:
	//private:
	//	short m_x;
	//	short m_y;
	//};

	//typedef std::pair< short, short > Location;

	//union Location
	//{
	//	short s[2];
	//	int i;
	//};

};