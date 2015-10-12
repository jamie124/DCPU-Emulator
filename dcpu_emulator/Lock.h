#pragma once

#include "Device.h"

class Lock : public Device
{
public:
	Lock();
	virtual ~Lock();

	bool init() override;
	void interrupt() override;
	void update() override;

private:
	bool _locked;
};

