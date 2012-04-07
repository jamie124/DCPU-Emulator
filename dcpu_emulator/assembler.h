#pragma once

#include "cpu.h"

const int MAX_CHARS = 1024;

typedef struct argumentStruct {
	argument_t argument;
	word_t nextWord;
	char* labelReference;		// If NULL, nextWord is valid, otherwise nextWord should point to this
	bool badArgument;
} argumentStruct_t;

typedef struct assembledInstruction {
	char* label;
	word_t* data;
	word_t dataLength;
	word_t address;
	opcode_t opcode;
	argumentStruct_t a;
	argumentStruct_t b;
	struct assembledInstruction* next;
} assembledInstruction_t;

class Assembler
{
private:
	opcode_t opcodeFor(char* command);
	nonbasicOpcode_t nonbasicOpcodeFor(char* command);
	int registerFor(char regName);
	argumentStruct_t argumentFor(char* arg);


public:
	Assembler(void);
	~Assembler(void);

	int compile(char* filename);
};
