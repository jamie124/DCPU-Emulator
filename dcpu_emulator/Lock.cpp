#include "stdafx.h"
#include "Lock.h"


Lock::Lock() :
	_locked(false)
{
	_name = "Basic Lock";
	_identifier = 0x33d0c6a2;
	_version = 1;
	_manufacturer = 0x10011009;
}


Lock::~Lock()
{
}

bool Lock::init()
{
	return true;
}

void Lock::interrupt()
{
	auto aVal = _cpu->getRegister(0);
	auto bVal = _cpu->getRegister(1);

	switch (aVal) {
	case 0:
		if (bVal != 0) {
			_locked = true;
		}
		else {
			_locked = false;
		}
		break;
	case 1:
		_cpu->setRegister(2, _locked);
		break;
	}
}

void Lock::update()
{

}
