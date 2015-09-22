/**
DCPU Emulator.
Written by James Whitwell, 2012.

CPU emulation class
This code orginally based on dcpu-emu https://bitbucket.org/interfect/dcpu-emu

Started 7-Apr-2012
*/

#include "StdAfx.h"
#include "cpu.h"
#include <iostream>
#include <fstream>
#include <stdio.h>


word_t* registers;

word_t programCounter;
word_t stackPointer;
word_t overflow;

word_t cycle;

word_t keyboardPosition;

word_t* colourTable;

Cpu::Cpu()
{
	DEBUG = false;
	OPCODE_DEBUGGING = true;

	_memory.resize(MEMORY_LIMIT);


	registers = new word_t[NUM_REGISTERS];
	for (word_t i = 0; i < NUM_REGISTERS; i++) {
		registers[i] = 0;
	}

	// Colour table
	colourTable = new word_t[NUM_COLOURS];
	colourTable[0] = 0; // Black
	colourTable[1] = 4; // Blue
	colourTable[2] = 2; // Green
	colourTable[3] = 6; // Cyan
	colourTable[4] = 1; // Red
	colourTable[5] = 5; // Magenta
	colourTable[6] = 3; // Brown
	colourTable[7] = 7; // Light Gray
	colourTable[8] = 8; // Gray
	colourTable[9] = 12; // Light Blue
	colourTable[10] = 10; // Light Green
	colourTable[11] = 14; // Light Cyan
	colourTable[12] = 9; // Light Red
	colourTable[13] = 13; // Light Magenta
	colourTable[14] = 11; // Yellow
	colourTable[15] = 15; // White


	programCounter = 0x0;
	stackPointer = 0x0;
	overflow = 0;

	cycle = 0;
}


Cpu::~Cpu()
{
	delete registers;
	delete colourTable;
}

int Cpu::run(std::string filename)
{
	clearScreen();


	std::ifstream input(filename.c_str(), std::ios::binary);

	uint32_t index = 0;

	if (input.is_open()) {

		while (!input.eof()) {

			input.read(reinterpret_cast<char*>(&_memory.at(index++)), sizeof word_t);

		}

	}

	input.close();

	bool videoDirty = false;

	int pause;

	while (1) {

		std::cin.get();


		word_t executingPC = programCounter;
		instruction_t instruction = _memory[programCounter++];

		// Decode
		opcode_t opcode = getOpcode(instruction);
		nonbasicOpcode_t nonbasicOpcode;

		word_t aLoc;
		word_t bLoc;
		bool skipStore;

		auto aArg = getArgument(instruction, 0);
		auto bArg = getArgument(instruction, 1);

		if (opcode == OP_NONBASIC) {
			nonbasicOpcode = (nonbasicOpcode_t)getArgument(instruction, 0);

		//	evaluateArgument(getArgument(instruction, 1), aLoc);
			bLoc = getValue(bArg, false);
			skipStore = 1;
		}
		else {
			aLoc = getValue(aArg, false);
			bLoc = getValue(bArg, true);
			skipStore = isConst(getArgument(instruction, 0));		// If literal
		}
		word_t result = 0;

		// Execute
		unsigned int resultWithCarry;		// Some opcodes use internal variable
		bool skipNext = 0;				// Skip the next instruction

		switch (opcode) {
		case OP_NONBASIC:
			skipStore = 1;

			switch (nonbasicOpcode) {
			case OP_JSR:
				// 0x01 JSR - pushes the address of next instruction onto stack.
				// Sets PC to A
			//	_memory[--stackPointer] = programCounter;
			//	programCounter = aLoc;
				
				stackPointer = (stackPointer - 1) & 0xffff;
				_memory.at(stackPointer) = programCounter;
				programCounter = bLoc;
				cycle += 3;
				break;
			default:
				std::cout << "ERROR: Reserved OP_NONBASIC" << std::endl;
				return -2;
			}

			break;

		case OP_SET:
			// Set value of A to B
			result = bLoc;
			cycle += 1;

			if (OPCODE_DEBUGGING) {
				setCursorPos(TERM_WIDTH + 6, 2);
				std::cout << "SET A to " << bLoc;
			}
			break;

		case OP_ADD:
			// Add B to A, sets O
			result = aLoc + bLoc;
			overflow = (result < aLoc || result < bLoc);
			cycle += 2;

			if (OPCODE_DEBUGGING) {
				setCursorPos(TERM_WIDTH + 6, 2);
				std::cout << "ADD " << bLoc << " to A";
			}
			break;

		case OP_SUB:
			// Subtracts B from a, sets O
			result = aLoc - bLoc;
			overflow = (result > aLoc) ? 0xFFFF : 0;
			cycle += 2;

			if (OPCODE_DEBUGGING) {
				setCursorPos(TERM_WIDTH + 6, 2);
				std::cout << "SUB " << bLoc << " FROM A";
			}
			break;

		case OP_MUL:
			// Multiple A by B, set O
			resultWithCarry = (unsigned int)aLoc * (unsigned int)bLoc;
			result = (word_t)(resultWithCarry & 0xFFFF);	// Low word
			overflow = (word_t)(resultWithCarry >> 16);	// High word
			cycle += 2;

			if (OPCODE_DEBUGGING) {
				setCursorPos(TERM_WIDTH + 6, 2);
				std::cout << "MUL A by " << bLoc;
			}
			break;

		case OP_DIV:
			// Divide A by B, set O
			if (bLoc != 0) {
				resultWithCarry = ((unsigned int)aLoc << 16) / (unsigned int)bLoc;
				result = (word_t)(resultWithCarry >> 16);		// High word
				overflow = (word_t)(resultWithCarry & 0xFFFF);	// Low word
			}
			else {
				result = 0;
				overflow = 0;
			}

			cycle += 3;

			if (OPCODE_DEBUGGING) {
				setCursorPos(TERM_WIDTH + 6, 2);
				std::cout << "DIV A by " << bLoc;
			}
			break;

		case OP_MOD:
			// Remainder of A over B
			if (bLoc != 0) {
				result = aLoc % bLoc;
			}
			else {
				result = 0;
			}

			cycle += 3;

			if (OPCODE_DEBUGGING) {
				setCursorPos(TERM_WIDTH + 6, 2);
				std::cout << "Remainder of A over  " << bLoc;
			}
			break;

		case OP_SHL:
			// Shift A left B places, set O
			overflow = ((aLoc << bLoc) >> 16) & 0xffff;
			result = (aLoc << bLoc);
			//	resultWithCarry = (unsigned int) aLoc << bLoc;
			//	result = (word_t) (resultWithCarry & 0xFFFF);
			//	overflow = (word_t) (resultWithCarry >> 16);
			cycle += 2;
			break;

		case OP_SHR:
			// Shift A right B places, set O
			overflow = ((aLoc << 16) >> bLoc) & 0xffff;
			result = (aLoc >> bLoc);

			//resultWithCarry = (unsigned int) aLoc >> bLoc;
			//result = (word_t) (resultWithCarry >> 16);
		//	overflow = (word_t) (resultWithCarry & 0xFFFF);
			cycle += 2;
			break;

		case OP_AND:
			// Binary AND of A and B
			result = aLoc & bLoc;
			cycle += 1;
			break;

		case OP_BOR:
			// Binary OR of A and B
			result = bLoc | aLoc;
			cycle += 1;
			break;

		case OP_XOR:
			// Binary XOR of A and B
			result = aLoc ^ bLoc;
			cycle += 1;
			break;

		case OP_IFE:
			// Skip next instruction if A != B
			skipStore = 1;
			skipNext = !!(aLoc != bLoc);
			cycle += (2 + skipNext);		// 2, +1 if skipped
			break;

		case OP_IFN:
			// Skip next instruction if A == B
			skipStore = 1;
			skipNext = !!(aLoc == bLoc);
			cycle += (2 + skipNext);
			break;

		case OP_IFG:
			// Skip next instruction if A <= B
			skipStore = 1;
			skipNext = !!(aLoc <= bLoc);
			cycle += (2 + skipNext);
			break;

		case OP_IFB:
			// Skip next instruction if (A & B) == 0
			skipStore = 1;
			skipNext = (!(aLoc & bLoc));
			cycle += (2 + skipNext);
			break;

		}

		// Store result back in A, if it's not being skipped
		if (!skipStore) {
			// Halt?
			if (&aLoc == &programCounter && result == executingPC
				&& ((opcode == OP_SET && getArgument(instruction, 1) == ARG_NEXTWORD_LITERAL)
					|| (opcode == OP_SUB && getArgument(instruction, 1) == ARG_LITERAL_START + 1))) {
				std::cout << "SYSTEM HALTED!" << std::endl;
				return -1;
			}

			// Check if video needs to be updated
			if (&aLoc >= &_memory[CONSOLE_START] && &aLoc < &_memory[CONSOLE_END]) {
				videoDirty = true;
			}

			aLoc = result;
		}

		// Skip next instruction if needed
		if (skipNext) {
			programCounter += getInstructionLength(_memory[programCounter]);
		}

		if (!skipStore) {
			setValue(aArg, result);
		}

		// TODO: Update video memory

	//	if (videoDirty) {
			//clearScreen();
			for (int i = 0; i < TERM_HEIGHT; i++) {
				for (int j = 0; j < TERM_WIDTH; j += 1) {
					word_t toPrint = _memory[CONSOLE_START + i * TERM_WIDTH + j];

					setScreen(i, j, toPrint);
				}
			}

	//		videoDirty = false;
	//	}


		setCursorPos(1, TERM_HEIGHT + 1);
		printf("==== Program Status - CYCLE 0x%04hx====\n", cycle);
		printf("A:  0x%04hx\tB:  0x%04hx\tC:  0x%04hx\n", registers[0], registers[1], registers[2]);
		printf("X:  0x%04hx\tY:  0x%04hx\tZ:  0x%04hx\n", registers[3], registers[4], registers[5]);
		printf("I:  0x%04hx\tJ:  0x%04hx\n", registers[6], registers[7]);
		printf("PC: 0x%04hx\tSP: 0x%04hx\tO:  0x%04hx\n", programCounter, stackPointer, overflow);
		printf("Instruction: 0x%04hx\n", instruction);

		// Print part of stack
		for (int i = 0x8000; i < (0x800A); ++i) {
			printf("0x%04hx,\t", _memory[i]);
		}
	//	for (int i = 0xffff; i > (0xfff0); --i) {
	//		printf("0x%04hx,\t", _memory[i]);
	//	}

	}

	return 1;
}


word_t Cpu::getValue(argument_t argument, bool argA)
{
	if (argument < ARG_REG_END) {
		auto regNum = argument - ARG_REG_START;
		return registers[regNum];

	}
	else if (argument < ARG_REG_INDEX_END) {
		auto regNum = argument - ARG_REG_INDEX_START;
		auto reg = registers[regNum];

		return _memory[reg];
	}
	else if (argument < ARG_REG_NEXTWORD_INDEX_END) {
		auto nextWord = _memory[programCounter];
		programCounter = (programCounter + 1) & 0xffff;

		cycle += 1;

		auto regNum = argument - ARG_REG_INDEX_END;
		auto reg = registers[regNum];

		return _memory[(nextWord + reg) & 0xffff];

	}
	else if (argument == ARG_PUSH_POP) {
		if (argA) {
			// Pop
			word_t value = _memory[stackPointer];
			stackPointer = (stackPointer + 1) & 0xffff;
			
			return value;

		} else {
			// Push
			word_t value = _memory[(stackPointer - 1) & 0xffff];

			stackPointer = (stackPointer - 1) & 0xffff;

			return value;
		}
	}
	else if (argument == ARG_PEEK) {
		return _memory[stackPointer];
	}
	else if (argument == ARG_SP) {
		return stackPointer;
	}
	else if (argument == ARG_PC) {
		return programCounter;
	}
	else if (argument == ARG_O) {
		return overflow;
	}
	else if (argument == ARG_NEXTWORD_LITERAL) {
		word_t nextWord = _memory.at(programCounter);
		cycle += 1;

		programCounter = (programCounter + 1) & 0xffff;
		return nextWord;
	}
	else {
		return (argument - 0x21) & 0xffff;
	}
}

void Cpu::setValue(argument_t argument, word_t value)
{
	if (argument < ARG_REG_END) {
		auto regNum = argument - ARG_REG_START;

		registers[regNum] = value;

	}
	else if (argument < ARG_REG_INDEX_END) {
		auto regNum = argument - ARG_REG_INDEX_START;
		auto reg = registers[regNum];

		_memory[reg] = value;
	}
	else if (argument < ARG_REG_NEXTWORD_INDEX_END) {
		auto regNum = argument - ARG_REG_INDEX_END;
		auto reg = registers[regNum];

		auto nextWord = _memory[(programCounter - 1) & 0xffff];
		_memory[(nextWord + reg) & 0xffff] = value;
	}
	else if (argument == ARG_PUSH_POP) {
		_memory[stackPointer] = value;
	}
	else if (argument == ARG_PC) {
		programCounter = value;
	}

	std::cout << _memory[stackPointer];

}

// Get an opcode from instruction
opcode_t Cpu::getOpcode(instruction_t instruction)
{
	return instruction & 0x1F;
}

argument_t Cpu::getArgument(instruction_t instruction, boolean argB)
{
	// First 6 bits for true, second 6 for false
//	return ((instruction >> 4) >> 6 * which) & 0x3F;

	if (argB) {
		return (instruction >> 10) & 0x3f;
	}
	else {
		return (instruction >> 5) & 0x1f;
	}

	//	return (instruction >> (which ? 5 : 10)) & 0x3F;
}

instruction_t Cpu::setOpcode(instruction_t instruction, opcode_t opcode)
{
	// Clear low 4 bits and OR in opcode
	return (instruction & 0xFFF0) | opcode;
}

instruction_t Cpu::setArgument(instruction_t instruction, bool_t which, argument_t argument)
{
	if (!which) {
		// A argument
		return (instruction & 0xFC0F) | (((word_t)argument) << 4);
	}
	else {
		// B argument
		return (instruction & 0x03FF) | (((word_t)argument) << 10);
	}
}

// Check if argument references next word
bool_t Cpu::usesNextWord(argument_t argument)
{
	return (argument >= ARG_REG_NEXTWORD_INDEX_START && argument < ARG_REG_NEXTWORD_INDEX_END)
		|| argument == ARG_NEXTWORD
		|| argument == ARG_NEXTWORD_LITERAL;
}

// Is argument constant
bool_t Cpu::isConst(argument_t argument)
{
	return (argument >= ARG_LITERAL_START && argument < ARG_LITERAL_END)
		|| argument == ARG_NEXTWORD_LITERAL;
}

// How many words does instruction take
word_t Cpu::getInstructionLength(instruction_t instruction)
{
	if (getOpcode(instruction) == OP_NONBASIC) {
		// 1 argument
		return 1 + usesNextWord(getArgument(instruction, 1));
	}
	else {
		return 1 + usesNextWord(getArgument(instruction, 0)) + usesNextWord(getArgument(instruction, 1));
	}
}

// Get offset from instruction for next word
word_t Cpu::getNextWordOffset(instruction_t instruction, bool_t which)
{
	if (getOpcode(instruction) == OP_NONBASIC) {
		// 1 argument, 1 extra word
		return (which == 0) && usesNextWord(getArgument(instruction, 1));
	}
	else {
		if (!usesNextWord(getArgument(instruction, which))) {
			return 0;
		}

		return 1 + (which && usesNextWord(getArgument(instruction, 0)));
	}
}

void Cpu::setScreen(word_t row, word_t column, word_t character)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	setCursorPos(column, row);

	word_t colourData = character >> 7;
	word_t foreColour = colourData >> 5;
	word_t backColour = (colourData >> 1) & 0xF;
	bool_t blinkBit = colourData & 0x1;

	char letter = (character & 0x7F);

	if (letter == '\0') {
		letter = ' ';
	}

	SetConsoleTextAttribute(console, 6);

	std::cout << letter;
}

void Cpu::setCursorPos(int x, int y)
{
	COORD pos = { x, y };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, 7);
	SetConsoleCursorPosition(console, pos);
}

void Cpu::clearScreen()
{
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
	SetConsoleCursorPosition(console, topLeft);
}