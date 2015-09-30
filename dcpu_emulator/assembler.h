#pragma once

#include "cpu.h"

typedef struct argumentStruct {
	argument_t argument;
	word_t nextWord;
	std::string labelReference;	
	bool badArgument;
	bool argAlreadySet;

	argumentStruct() :
	argAlreadySet(false) {}
} argumentStruct_t;

typedef struct assembledInstruction {
	std::string source;
	std::string label;
	std::vector<word_t> data;
	word_t address;
	opcode opcode;
	argumentStruct_t a;
	argumentStruct_t b;
} assembledInstruction_t;

using  AssembledInstructionPtr = std::shared_ptr<assembledInstruction_t>;

class Assembler
{
private:
	opcode opcodeFor(const std::string& command);
	nonbasic_opcode nonbasicOpcodeFor(const std::string& command);
	int registerFor(char regName);
	argumentStruct_t argumentFor(const std::string& arg, bool isB);

	int processLine(
		const std::string& currentLine, 
		std::string& data,
		std::string& label,
		bool& functionOnNextLine, 
		std::string& command, 
		std::string& arg1,
		std::string& arg2,
		bool containsLabel);

	int processCommand(
		const std::string& command, 
		std::string data,
		word_t& address, 
		const std::string& label,
		assembledInstruction_t& instruction);

	void processArg1(
		const std::string& command,
		const std::string& arg,
		word_t &address, 
		const std::string& label,
		assembledInstruction_t& instruction);

	void processArg2(
		const std::string& command,
		const std::string& arg,
		word_t &address, 
		const std::string& label,
		assembledInstruction_t& instruction);

public:
	Assembler();
	~Assembler();

	int compile(const std::string& filename);

	std::map<word_t, std::string> getLineMappings() const;

private:
	std::vector<assembledInstruction_t> _instructions;

	std::map<std::string, word_t> _foundLabels;
	std::map<word_t, std::string> _lineMappings;

	std::map<std::string, opcode> _opcodes;
	std::map<std::string, nonbasic_opcode> _nonbasicOpcode;
};

