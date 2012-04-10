// 0x10c Emulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "cpu.h"
#include "assembler.h"

int _tmain(int argc, _TCHAR* argv[])
{
	int pause;

	std::cout << "0x10c Emulator" << std::endl;

	Assembler* assembler = new Assembler();

	assembler->compile("demo_asm/loop.dasm16");

	delete assembler;

	Cpu* cpu = new Cpu();

	cpu->run("loop.bin");

	delete cpu;

	std::cin >> pause;

	return 0;
}

