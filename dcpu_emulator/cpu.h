/**
DCPU Emulator.
Written by James Whitwell, 2012.

CPU emulation class
This code orginally based on dcpu-emu https://bitbucket.org/interfect/dcpu-emu

Started 7-Apr-2012
*/
#pragma once


#include "Utils.h"

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
static const word_t ARG_REG_END = 0x08;
static const word_t ARG_REG_INDEX_START = 0x08;
static const word_t ARG_REG_INDEX_END = 0x10;
static const word_t ARG_REG_NEXTWORD_INDEX_START = 0x10;
static const word_t ARG_REG_NEXTWORD_INDEX_END = 0x18;
static const word_t ARG_PUSH_POP = 0x18;
static const word_t ARG_PEEK = 25;
static const word_t ARG_SP = 0x1b;
static const word_t ARG_PC = 0x1c;
static const word_t ARG_O = 0x1d;
static const word_t ARG_NEXTWORD = 0x1e;
static const word_t ARG_NEXTWORD_LITERAL = 0x1f;
static const word_t ARG_LITERAL_START = 0x20;
static const word_t ARG_LITERAL_END = 0x3f;

const long MEMORY_LIMIT = 0x10000;
const int NUM_REGISTERS = 8;

class Device;

class Cpu
{

public:
	Cpu();
	~Cpu();

	int run(const std::string& filename, std::map<word_t, std::string> lineMappings);

	static bool_t usesNextWord(argument_t argument);
	static instruction_t setOpcode(instruction_t instruction, opcode opcode);
	static instruction_t setArgument(instruction_t instruction, bool_t which, argument_t argument);

	void setScreen(word_t row, word_t column, word_t character);
	void setCursorPos(int x, int y);
	void clearScreen();

	word_t getRegister(word_t reg) const;
	void setRegister(word_t reg, word_t value);

	const std::vector<word_t> getMemory() const;

	bool isMemoryDirty() const;
	void resetMemoryDirty();

private:
	bool _debug;
	bool _opcodeDebugging;


	word_t _programCounter;
	word_t _stackPointer;
	word_t _overflow;

	word_t _cycle;

	bool _memoryDirty;

	std::vector<word_t> _memory;
	std::vector<word_t> _registers;

	std::string _currentDebugMessage;

	std::vector<std::unique_ptr<Device>> _devices;

	word_t getValue(argument_t argument, bool argA);
	void setValue(argument_t argument, word_t value);

	opcode getOpcode(instruction_t instruction);
	argument_t getArgument(instruction_t instruction, bool argB);

	bool_t isConst(argument_t argument);
	word_t getInstructionLength(instruction_t instruction);
	word_t getNextWordOffset(instruction_t instruction, bool_t which);
	word_t extendSign(word_t input);

	word_t to16BitSigned(int input);
	int to32BitSigned(word_t input);

	void debugInstruction(const std::string& message, word_t first, word_t second);
	void debugInstruction(const std::string& message, word_t first, word_t second, word_t result);
	
};

