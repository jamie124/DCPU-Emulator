#pragma once
#include "Device.h"

class LEM1820 :
	public Device
{
public:
	LEM1820();
	virtual ~LEM1820();

	bool init() override;
	void interrupt() override;

private:
	word_t _memOffset;

};

