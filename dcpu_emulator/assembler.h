#pragma once

#include "cpu.h"

#include <memory>

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
	std::shared_ptr<struct assembledInstruction> next;
} assembledInstruction_t;

using  AssembledInstructionPtr = std::shared_ptr<assembledInstruction_t>;

class Assembler
{
private:
	opcode_t opcodeFor(char* command);
	nonbasicOpcode_t nonbasicOpcodeFor(char* command);
	int registerFor(char regName);
	argumentStruct_t argumentFor(char* arg);

	char* cleanString(char *rawLine);
	int processLine(char *currentLine, char *data, char *label, bool &functionOnNextLine, char *command, char *arg1, char *arg2, bool containsLabel);

	int processCommand(char* command, char* data, word_t &address, char* label, 
		AssembledInstructionPtr &head, AssembledInstructionPtr &tail, AssembledInstructionPtr &instruction);
	void processArg1(char* command, char* arg, word_t &address, char* label, AssembledInstructionPtr &instruction);
	void processArg2(char* command, char* arg, word_t &address, char* label, AssembledInstructionPtr &instruction);

public:
	Assembler(void);
	~Assembler(void);

	int compile(const std::string& filename);
};

