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

word_t keyboardPosition;

Cpu::Cpu() :
	_programCounter(0),
	_stackPointer(0),
	_overflow(0),
	_cycle(0)
{
	_debug = false;
	_opcodeDebugging = true;

	_memory.resize(MEMORY_LIMIT);

	_registers.resize(NUM_REGISTERS);

	for (word_t i = 0; i < NUM_REGISTERS; i++) {
		_registers.at(i) = 0;
	}
}

Cpu::~Cpu()
{
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


		word_t executingPC = _programCounter;
		instruction_t instruction = _memory[_programCounter++];

		// Decode
		opcode_t opcode = getOpcode(instruction);
		nonbasicOpcode_t nonbasicOpcode;

		word_t bLoc;
		word_t aLoc;
		bool skipStore;

		auto bArg = getArgument(instruction, false);
		auto aArg = getArgument(instruction, true);

		if (opcode == OP_NONBASIC) {
			nonbasicOpcode = (nonbasicOpcode_t)getArgument(instruction, 0);

		//	evaluateArgument(getArgument(instruction, 1), aLoc);
			aLoc = getValue(aArg, false);
			skipStore = 1;
		}
		else {
			bLoc = getValue(bArg, false);
			aLoc = getValue(aArg, true);
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
				
				_stackPointer = (_stackPointer - 1) & 0xffff;
				_memory.at(_stackPointer) = _programCounter;
				_programCounter = aLoc;
				_cycle += 3;
				break;
			default:
				std::cout << "ERROR: Reserved OP_NONBASIC" << std::endl;
				return -2;
			}

			break;

		case OP_SET:
			// Set value of A to B
			result = aLoc;
			_cycle += 1;

			if (_opcodeDebugging) {
				debugInstruction("SET %x to %x", bLoc, aLoc);
			}

			break;

		case OP_ADD:
			// Add B to A, sets O
			result = bLoc + aLoc;
			_overflow = (result < bLoc || result < aLoc);
			_cycle += 2;

			if (_opcodeDebugging) {
				debugInstruction("ADD %x to %x = %x", aLoc, bLoc, result);
			}

			break;

		case OP_SUB:
			// Subtracts B from a, sets O
			result = bLoc - aLoc;
			_overflow = (result > bLoc) ? 0xFFFF : 0;
			_cycle += 2;

			if (_opcodeDebugging) {
				debugInstruction("SUB %x from %x = %x", aLoc, bLoc, result);
			}

			break;

		case OP_MUL:
		{
			// Multiple A by B, set O
			auto temp = ((unsigned int)bLoc * (unsigned int)aLoc);
			result = to16BitSigned(temp);
			_overflow = (word_t)((temp >> 16) & 0xffff);
			_cycle += 2;

			if (_opcodeDebugging) {
				debugInstruction("MUL %x by %x = %x", bLoc, aLoc, result);
			}
		}
			break;

		case OP_MLI:
		{
			auto temp  = (to32BitSigned(bLoc) * to32BitSigned(aLoc));
			result = to16BitSigned(temp);
			_overflow = (word_t)(temp >> 16) & 0xffff;
			_cycle += 2;

			if (_opcodeDebugging) {
				debugInstruction("MLI %x by %x = %x", bLoc, aLoc, result);
			}
		}
			break;

		case OP_DIV:
		{
			// Divide A by B, set O
			if (aLoc != 0) {
				result = to32BitSigned((unsigned int)bLoc / (unsigned int)aLoc);
				_overflow = to16BitSigned((bLoc << 16) / aLoc);
			}
			else {
				result = 0;
				_overflow = 0;
			}

			_cycle += 3;

			if (_opcodeDebugging) {
				debugInstruction("DIV %x by %x = %x", bLoc, aLoc, result);
			}

			break;
		}
		case OP_DVI: 
		{
			if (aLoc != 0) {
				result = to16BitSigned(to32BitSigned(bLoc) / to32BitSigned(aLoc));
				_overflow = to16BitSigned((bLoc << 16) / aLoc);
			}
			else {
				result = 0;
				_overflow = 0;
			}

			_cycle += 3;

			if (_opcodeDebugging) {
				debugInstruction("DVI %x by %x = %x", bLoc, aLoc, result);
			}
			break;
		}
		case OP_MOD:
			// Remainder of A over B
			if (aLoc != 0) {
				result = bLoc % aLoc;
			}
			else {
				result = 0;
			}

			_cycle += 3;

			if (_opcodeDebugging) {
				debugInstruction("MOD %x mod %x = %x", bLoc, aLoc, result);
			}
			break;

		case OP_MDI:
			if (aLoc == 0) {
				result = 0;
			}
			else {
				result = to16BitSigned(to32BitSigned(bLoc) % to32BitSigned(aLoc));
			}

			_cycle += 3;

			if (_opcodeDebugging) {
				debugInstruction("MDI %x mod %x = %x", bLoc, aLoc, result);
			}

			break;

		case OP_AND:
			// Binary AND of A and B
			result = bLoc & aLoc;
			_cycle += 1;

			if (_opcodeDebugging) {
				debugInstruction("AND %x & %x = %x", bLoc, aLoc, result);
			}
			break;

		case OP_BOR:
			// Binary OR of A and B
			result = aLoc | bLoc;
			_cycle += 1;

			if (_opcodeDebugging) {
				debugInstruction("BOR %x | %x = %x", bLoc, aLoc, result);
			}
			break;

		case OP_XOR:
			// Binary XOR of A and B
			result = bLoc ^ aLoc;
			_cycle += 1;

			if (_opcodeDebugging) {
				debugInstruction("XOR %x ^ %x = %x", bLoc, aLoc, result);
			}

			break;

		case OP_SHR:
			// Shift A right B places, set O
			_overflow = to16BitSigned((bLoc << 16) >> aLoc);
			result = (bLoc >> aLoc);

			_cycle += 1;
			break;

		case OP_ASR:
		{
			int temp = to32BitSigned(bLoc);

			_overflow = to16BitSigned((temp << 16) >> aLoc);
			result = (temp >> aLoc);

			_cycle += 1;
			break;
		}
		case OP_SHL:
			// Shift A left B places, set O
			_overflow = to16BitSigned((bLoc << aLoc) >> 16);
			result = (bLoc << aLoc);
	
			_cycle += 1;
			break;

		case OP_IFE:
			// Skip next instruction if A != B
			skipStore = 1;
			skipNext = !!(bLoc != aLoc);
			_cycle += (2 + skipNext);		// 2, +1 if skipped
			break;

		case OP_IFN:
			// Skip next instruction if A == B
			skipStore = 1;
			skipNext = !!(bLoc == aLoc);
			_cycle += (2 + skipNext);
			break;

		case OP_IFG:
			// Skip next instruction if A <= B
			skipStore = 1;
			skipNext = !!(bLoc <= aLoc);
			_cycle += (2 + skipNext);
			break;

		case OP_IFB:
			// Skip next instruction if (A & B) == 0
			skipStore = 1;
			skipNext = (!(bLoc & aLoc));
			_cycle += (2 + skipNext);
			break;

		default:
			debugInstruction("Unknown instruction %x", opcode, 0);
			break;

		}

		// Store result back in A, if it's not being skipped
		if (!skipStore) {
			// Halt?
			if (&bLoc == &_programCounter && result == executingPC
				&& ((opcode == OP_SET && getArgument(instruction, 1) == ARG_NEXTWORD_LITERAL)
					|| (opcode == OP_SUB && getArgument(instruction, 1) == ARG_LITERAL_START + 1))) {
				std::cout << "SYSTEM HALTED!" << std::endl;
				return -1;
			}

			// Check if video needs to be updated
		//	if (&bLoc >= &_memory[CONSOLE_START] && &bLoc < &_memory[CONSOLE_END]) {
		//		videoDirty = true;
		//	}

			bLoc = result;
		}

		// Skip next instruction if needed
		if (skipNext) {
			_programCounter += getInstructionLength(_memory[_programCounter]);
		}

		if (!skipStore) {
			setValue(bArg, result);
		}

		// TODO: Update video memory

	//	if (videoDirty) {
			clearScreen();
			for (int i = 0; i < TERM_HEIGHT; i++) {
				for (int j = 0; j < TERM_WIDTH; j += 1) {
					word_t toPrint = _memory[CONSOLE_START + i * TERM_WIDTH + j];

					setScreen(i, j, toPrint);
				}
			}

	//		videoDirty = false;
	//	}


		setCursorPos(1, TERM_HEIGHT + 1);
		printf("==== Program Status - CYCLE 0x%04hx====\n", _cycle);
		printf("A:  0x%04hx\tB:  0x%04hx\tC:  0x%04hx\n", 
			_registers.at(0), 
			_registers.at(1),
			_registers.at(2));
		printf("X:  0x%04hx\tY:  0x%04hx\tZ:  0x%04hx\n", 
			_registers.at(3),
			_registers.at(4),
			_registers.at(5));
		printf("I:  0x%04hx\tJ:  0x%04hx\n", 
			_registers.at(6),
			_registers.at(7));
		printf("PC: 0x%04hx\tSP: 0x%04hx\tEX:  0x%04hx\n", _programCounter, _stackPointer, _overflow);
		printf("Instruction: 0x%04hx\n", instruction);
		std::cout << _currentDebugMessage.c_str() << std::endl;

		// Print part of stack
		for (int i = 0xffff; i > (0xfff0); --i) {
			printf("0x%04hx,\t", _memory[i]);
		}

	}

	return 1;
}


word_t Cpu::getValue(argument_t argument, bool argA)
{
	if (argument < ARG_REG_END) {
		auto regNum = argument - ARG_REG_START;
		return _registers.at(regNum);

	}
	else if (argument < ARG_REG_INDEX_END) {
		auto regNum = argument - ARG_REG_INDEX_START;
		auto reg = _registers.at(regNum);

		return _memory[reg];
	}
	else if (argument < ARG_REG_NEXTWORD_INDEX_END) {
		auto nextWord = _memory[_programCounter];
		_programCounter = (_programCounter + 1) & 0xffff;

		_cycle += 1;

		auto regNum = argument - ARG_REG_INDEX_END;
		auto reg = _registers.at(regNum);

		return _memory[(nextWord + reg) & 0xffff];

	}
	else if (argument == ARG_PUSH_POP) {
		if (argA) {
			// Pop
			word_t value = _memory[_stackPointer];
			_stackPointer = (_stackPointer + 1) & 0xffff;
			
			return value;

		} else {
			// Push
			word_t value = _memory[(_stackPointer - 1) & 0xffff];

			_stackPointer = (_stackPointer - 1) & 0xffff;

			return value;
		}
	}
	else if (argument == ARG_PEEK) {
		return _memory[_stackPointer];
	}
	else if (argument == ARG_SP) {
		return _stackPointer;
	}
	else if (argument == ARG_PC) {
		return _programCounter;
	}
	else if (argument == ARG_O) {
		return _overflow;
	}
	else if (argument == ARG_NEXTWORD_LITERAL) {
		word_t nextWord = _memory.at(_programCounter);
		_cycle += 1;

		_programCounter = (_programCounter + 1) & 0xffff;
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

		_registers.at(regNum) = value;

	}
	else if (argument < ARG_REG_INDEX_END) {
		auto regNum = argument - ARG_REG_INDEX_START;
		auto reg = _registers.at(regNum);

		_memory[reg] = value;
	}
	else if (argument < ARG_REG_NEXTWORD_INDEX_END) {
		auto regNum = argument - ARG_REG_INDEX_END;
		auto reg = _registers.at(regNum);

		auto nextWord = _memory[(_programCounter - 1) & 0xffff];
		_memory[(nextWord + reg) & 0xffff] = value;
	}
	else if (argument == ARG_PUSH_POP) {
		_memory[_stackPointer] = value;
	}
	else if (argument == ARG_PC) {
		_programCounter = value;
	}


}

// Get an opcode from instruction
opcode_t Cpu::getOpcode(instruction_t instruction)
{
	return instruction & 0x1F;
}

argument_t Cpu::getArgument(instruction_t instruction, bool argB)
{
	if (argB) {
		return (instruction >> 10) & 0x3f;
	}
	else {
		return (instruction >> 5) & 0x1f;
	}

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


word_t Cpu::extendSign(word_t input)
{
	if (input & 0x8000) {
		// Negative
		return input | 0xffff0000;
	}
	else {
		return input;
	}
}

word_t Cpu::to16BitSigned(int input)
{
	if (input < 0) {
		return ((input & 0x7fff) | 0x8000);
	}
	else {
		return input & 0xffff;
	}
}

int Cpu::to32BitSigned(word_t input)
{
	if ((input & 0x8000) > 0) {
		return (((~input) + 1) & 0xffff) * -1;
	}
	else {
		return input;
	}
}

void Cpu::debugInstruction(const std::string& message, word_t first, word_t second)
{
	_currentDebugMessage = string_format(message, first, second);
}

void Cpu::debugInstruction(const std::string& message, word_t first, word_t second, word_t result)
{
	_currentDebugMessage = string_format(message, first, second, result);
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