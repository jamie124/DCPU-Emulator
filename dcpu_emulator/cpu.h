/**
DCPU Emulator.
Written by James Whitwell, 2012.

CPU emulation class
This code orginally based on dcpu-emu https://bitbucket.org/interfect/dcpu-emu

Started 7-Apr-2012
*/
#pragma once

#include <iostream>
#include <vector>

#include <Windows.h>


using word_t = unsigned short;
using instruction_t = word_t;

using argument_t = unsigned char;
using opcode_t = unsigned char;
using nonbasicOpcode_t = argument_t;

using bool_t = unsigned char;

static const int OP_NONBASIC = 0x0;
static const int OP_SET = 0x01;
static const int OP_ADD = 0x02;
static const int OP_SUB = 0x03;
static const int OP_MUL = 0x04;
static const int OP_MLI = 0x05;
static const int OP_DIV = 0x06;
static const int OP_DVI = 0x07;
static const int OP_MOD = 0x08;
static const int OP_MDI = 0x09;
static const int OP_AND = 0x0a;
static const int OP_BOR = 0x0b;
static const int OP_XOR = 0x0c;
static const int OP_SHR = 0x0d;
static const int OP_ASR = 0x0e;
static const int OP_SHL = 0x0f;
static const int OP_IFB = 0x10;
static const int OP_IFC = 0x11;
static const int OP_IFE = 0x12;
static const int OP_IFN = 0x13;
static const int OP_IFG = 0x14;
static const int OP_IFA = 0x15;
static const int OP_IFL = 0x16;
static const int OP_IFU = 0x17;
// Reserved 0x18 and 0x19
static const int OP_ADX = 0x1a;
static const int OP_SBX = 0x1b;
// Reserved 0x1c and 0x1d
static const int OP_STI = 0x1e;
static const int OP_STD = 0x1f;

// Special opcodes
// Reserved 0x00
static const int OP_JSR = 0x01;
// Reserved 0x02 - 0x07
static const int OP_INT = 0x08;
static const int OP_IAG = 0x09;
static const int OP_IAS = 0x0a;
static const int OP_RFI = 0x0b;
static const int OP_IAQ = 0x0c;
// Reserved 0x0d - 0x0f
static const int OP_HWN = 0x10;
static const int OP_HWQ = 0x11;
static const int OP_HWI = 0x12;
// Reserved 0x13 - 0x1f

static const word_t ARG_REG_START = 0;
static const word_t ARG_REG_END = 8;
static const word_t ARG_REG_INDEX_START = 8;
static const word_t ARG_REG_INDEX_END = 16;
static const word_t ARG_REG_NEXTWORD_INDEX_START = 16;
static const word_t ARG_REG_NEXTWORD_INDEX_END = 24;
static const word_t ARG_PUSH_POP = 0x18;
static const word_t ARG_PEEK = 25;
//static const word_t ARG_PUSH = 26;
static const word_t ARG_SP = 0x1b;
static const word_t ARG_PC = 0x1c;
static const word_t ARG_O = 0x1d;
static const word_t ARG_NEXTWORD = 0x1e;
static const word_t ARG_NEXTWORD_LITERAL = 0x1f;
static const word_t ARG_LITERAL_START = 0x20;
static const word_t ARG_LITERAL_END = 0x3f;

const long MEMORY_LIMIT = 0x10000;
const int NUM_REGISTERS = 8;

const int TERM_WIDTH = 32;
const int TERM_HEIGHT = 16;
const long CONSOLE_START = 0x8000;
const long CONSOLE_END = (CONSOLE_START + TERM_WIDTH + TERM_HEIGHT);

// Keyboard input
const long KEYBOARD_ADDRESS = 0x9000;
const int KEYBOARD_BUFFER_LENGTH = 1;

const int FRAMESKIP = 10;

const int NUM_COLOURS = 16;

class Cpu
{

public:
	Cpu();
	~Cpu();

	int run(std::string filename);

	static bool_t usesNextWord(argument_t argument);
	static instruction_t setOpcode(instruction_t instruction, opcode_t opcode);
	static instruction_t setArgument(instruction_t instruction, bool_t which, argument_t argument);

	void setScreen(word_t row, word_t column, word_t character);
	void setCursorPos(int x, int y);
	void clearScreen();

private:
	bool DEBUG;
	bool OPCODE_DEBUGGING;

	bool STEP_MODE;
	bool RUNNING;

	std::vector<word_t> _memory;

//	word_t* evaluateArgument(argument_t argument);

//	void evaluateArgument(argument_t argument, word_t& argumentResult);

	word_t getValue(argument_t argument, bool argA);
	void setValue(argument_t argument, word_t value);

	opcode_t getOpcode(instruction_t instruction);
	argument_t getArgument(instruction_t instruction, boolean argB);

	bool_t isConst(argument_t argument);
	word_t getInstructionLength(instruction_t instruction);
	word_t getNextWordOffset(instruction_t instruction, bool_t which);

};

