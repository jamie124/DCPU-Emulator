#pragma once

#include "cpu.h"

#include <algorithm>
#include <memory>
#include <vector>

const int MAX_CHARS = 1024;

typedef struct argumentStruct {
	argument_t argument;
	word_t nextWord;
	std::string labelReference;		// If NULL, nextWord is valid, otherwise nextWord should point to this
	bool badArgument;
} argumentStruct_t;

typedef struct assembledInstruction {
	std::string label;
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
	opcode_t opcodeFor(const std::string& command);
	nonbasicOpcode_t nonbasicOpcodeFor(const std::string& command);
	int registerFor(char regName);
	argumentStruct_t argumentFor(const std::string& arg);

	char* cleanString(char *rawLine);

	std::string& ltrim(std::string& str);
	std::string& rtrim(std::string& str);
	std::string& trim(std::string& str);

	unsigned int split(const std::string &txt, std::vector<std::string> &strs, char splitChar);

	std::string replace(const std::string& input, char from, char to);

	std::string toLower(const std::string& input);

	int processLine(
		const std::string& currentLine, 
		std::string& data,
		std::string& label,
		bool &functionOnNextLine, 
		std::string& command, 
		std::string& arg1,
		std::string& arg2,
		bool containsLabel);

	int processCommand(
		const std::string& command, 
		std::string data,
		word_t &address, 
		const std::string& label,
		AssembledInstructionPtr &head, 
		AssembledInstructionPtr &tail, 
		AssembledInstructionPtr &instruction);

	void processArg1(
		const std::string& command,
		const std::string& arg,
		word_t &address, 
		const std::string& label,
		AssembledInstructionPtr &instruction);

	void processArg2(
		const std::string& command,
		const std::string& arg,
		word_t &address, 
		const std::string& label,
		AssembledInstructionPtr &instruction);

public:
	Assembler(void);
	~Assembler(void);

	int compile(const std::string& filename);
};

