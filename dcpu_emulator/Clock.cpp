#include "stdafx.h"
#include "Clock.h"


Clock::Clock() :
	_elapsed(0),
	_duration(0),

	_interruptsOn(false),
	_interruptMessage(0),
	_running(false)
{
	_name = "Clock";
	_identifier = 0x12d0b402;
	_version = 1;
	_manufacturer = 0x90099009;
}


Clock::~Clock()
{
}

bool Clock::init()
{
	return true;
}

void Clock::interrupt()
{
	auto aVal = _cpu->getRegister(0);
	auto bVal = _cpu->getRegister(1);

	switch (aVal) {
	case 0:
		if (bVal != 0) {
			start(bVal / 60 * 1000);
		}
		else {
			stop();
		}
		break;
	case 1:
		_cpu->setRegister(2, _elapsed);
		break;
	case 2:
		if (bVal != 0) {
			_interruptsOn = true;
			_interruptMessage = bVal;
		}
		else {
			_interruptsOn = false;
		}
		break;
	}
}

void Clock::update()
{
	if (_running) {
		auto elapsed = std::chrono::high_resolution_clock::now() - _start;

		if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() >= _duration) {
			_start = std::chrono::high_resolution_clock::now();

			_elapsed += 1;
		}
	}
}

void Clock::start(uint16_t duration)
{
	_running = true;
	_elapsed = 0;
	_duration = duration;

	_start = std::chrono::high_resolution_clock::now();
}

void Clock::tick()
{

}

void Clock::stop()
{

	_running = false;
}
