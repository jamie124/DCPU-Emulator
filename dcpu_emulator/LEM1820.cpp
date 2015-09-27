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

void LEM1820::interrupt()
{
	 auto aVal = _cpu->getRegister(0);
	 auto bVal = _cpu->getRegister(1);

	 switch (aVal) {
	 case 0:
		 if (bVal == 0) {
			 std::cout << "Screen disconnected";
		 }
		 else {
			 _memOffset = bVal;
		 }
		 break;
	 case 1:
		 // TODO - Map font
		 break;
	 case 2:
		 // TODO: Map palette
		 break;
	 case 3:
		 // TODO: Border colour
		 break;
	 case 4:
		 // TODO: Mem dump font
		 break;
	 case 5:
		 // TODO: Dump palette
		 break;
	 }
}
