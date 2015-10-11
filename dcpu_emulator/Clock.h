#pragma once

#include "Device.h"

class Clock : public Device
{
public:
	Clock();
	virtual ~Clock();

	bool init() override;
	void interrupt() override;
	void update() override;

private:
	void start(uint16_t duration);
	void tick();
	void stop();


	uint16_t _elapsed;
	uint16_t _duration;

	std::chrono::high_resolution_clock::time_point _start;

	bool _interruptsOn;
	word_t _interruptMessage;

	bool _running;

};

