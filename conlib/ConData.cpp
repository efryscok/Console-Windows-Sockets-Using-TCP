/** @file: ConData.cpp
	@author Erik Fryscok
	@date 2016-03-01
	@brief Source code for classes contained in the ConData namespace
	@note Project 2 - Singleton (INFO-5100 - Patterns in Software 2)
	*/

#include <ConData.hpp>

using namespace ConData;

// Coord constructor
Coord::Coord(std::initializer_list<SHORT> init) {
	X = *init.begin();
	Y = (init.size() > 1) ? *(init.begin() + 1) : *init.begin();
}

// SmallRect constructor
SmallRect::SmallRect(std::initializer_list<SHORT> init) {
	switch (init.size())
	{
	case 1:
		Top = Left = Bottom = Right = *init.begin();
		break;
	case 4:
		Left = *(init.begin() + 0);
		Top = *(init.begin() + 1);
		Right = *(init.begin() + 2);
		Bottom = *(init.begin() + 3);
		break;
	default:
		Top = Left = Bottom = Right = 0;
		break;
	}
}