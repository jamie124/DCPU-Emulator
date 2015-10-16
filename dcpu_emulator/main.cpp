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

	if (assembler->compile("D:\\Users\\James Whitwell\\Documents\\Unreal Projects\\Rebuild\\Content\\demo_asm", "import_test.dasm", false)) {

		std::unique_ptr<Cpu> cpu = std::make_unique<Cpu>();

		cpu->run("D:\\Users\\James Whitwell\\Documents\\Unreal Projects\\Rebuild\\Content\\demo_asm\\import_test.bin", assembler->getLineMappings());
	}
	
	std::cin >> pause;

	return 0;
}

