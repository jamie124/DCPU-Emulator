#pragma once

#include "Utils.h"

class Device
{
public:
	Device();
	virtual ~Device();

	virtual bool init() = 0;

	uint32_t getIdentifier() const;
	uint32_t getManufacturer() const;
	word_t getVersion() const;

protected:
	std::string _name;
	uint32_t _identifier;
	uint32_t _manufacturer;
	word_t _version;
};

