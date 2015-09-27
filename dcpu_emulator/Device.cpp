#include "stdafx.h"
#include "Device.h"


Device::Device()
{
}


Device::~Device()
{
}

uint32_t Device::getIdentifier() const
{
	return _identifier;
}

uint32_t Device::getManufacturer() const
{
	return _manufacturer;
}

word_t Device::getVersion() const
{
	return _version;
}
