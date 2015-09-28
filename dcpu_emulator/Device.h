#pragma once

#include "cpu.h"
#include "Utils.h"

class Device
{
public:
	Device();
	virtual ~Device();

	virtual bool init() = 0;
	virtual void interrupt() = 0;
	virtual void update() = 0;

	uint32_t getIdentifier() const;
	uint32_t getManufacturer() const;
	word_t getVersion() const;

	void setCpu(const Cpu* cpu);

protected:
	std::string _name;
	uint32_t _identifier;
	uint32_t _manufacturer;
	word_t _version;

	const Cpu* _cpu;
};

