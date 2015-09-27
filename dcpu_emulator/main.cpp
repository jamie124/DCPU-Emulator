// 0x10c Emulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <memory>

#include "cpu.h"
#include "assembler.h"

int _tmain(int argc, _TCHAR* argv[])
{
	int pause;

	std::cout << "0x10c Emulator" << std::endl;

	std::unique_ptr<Assembler> assembler = std::make_unique<Assembler>();

	if (assembler->compile("demo_asm\\basic.dasm16")) {

		std::unique_ptr<Cpu> cpu = std::make_unique<Cpu>();

		cpu->run("demo_asm\\basic.bin", assembler->getLineMappings());
	}
	
	std::cin >> pause;

	return 0;
}

