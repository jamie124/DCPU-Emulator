#include "stdafx.h"
#include "LEM1820.h"


LEM1820::LEM1820()
{
	_name = "LEM1802 - Low Energy Monitor";
	_identifier = 0x7349f615;
	_version = 0x1802;
	_manufacturer = 0x1c6c8b36;
}


LEM1820::~LEM1820()
{
}

bool LEM1820::init()
{
	std::cout << "Initialising LEM1820" << std::endl;

	return true;
}
