#include "StdAfx.h"

#include "assembler.h"
#include "Utils.h"

Assembler::Assembler()
{
	_opcodes = {
		{"set", OP_SET}, { "add", OP_ADD },{ "sub", OP_SUB },
		{ "mul", OP_MUL },{ "mli", OP_MLI },
		{ "div", OP_DIV },{"dvi", OP_DVI},
		{ "mod", OP_MOD },{ "mdi", OP_MDI },
		{ "add", OP_AND },{ "bor", OP_BOR },{ "xor", OP_XOR },
		{ "shr", OP_SHR },{ "asr", OP_ASR },{ "shl", OP_SHL },
		{"ifb", OP_IFB},{ "ifc", OP_IFC },{ "ife", OP_IFE },
		{ "ifn", OP_IFN },{ "ifg", OP_IFG },{ "ifa", OP_IFA},
		{ "ifl", OP_IFL },{ "ifu", OP_IFU },
		{ "adx", OP_ADX },{ "sbx", OP_SBX },
		{ "sti", OP_STI },{ "std", OP_STD }
	};

	_nonbasicOpcode = {
		{"jsr", OP_JSR},
		{"int", OP_INT},{ "iag", OP_IAG },{ "ias", OP_IAS },
		{ "rfi", OP_RFI },{ "iaq", OP_IAQ },
		{ "hwn", OP_HWN },{ "hwq", OP_HWQ },{ "hwi", OP_HWI }
	};
}


Assembler::~Assembler()
{
}

opcode Assembler::opcodeFor(const std::string& command)
{
	if (_opcodes.find(command) != _opcodes.end()) {
		return _opcodes.at(command);
	}

	// Assume non-basic
	return OP_NONBASIC;
}

// Get non-basic opcode from string
nonbasic_opcode Assembler::nonbasicOpcodeFor(const std::string& command)
{
	if (_nonbasicOpcode.find(command) != _nonbasicOpcode.end()) {
		return _nonbasicOpcode.at(command);
	}

	// Instruction not found
	std::cout << "ERROR: Unknown instruction \"" << command << "\"" << std::endl;
	return 0;
}

// Check register number 
int Assembler::registerFor(char regName)
{
	switch (regName) {
	case 'a':
	case 'A':
		return 0;
		break;
	case 'b':
	case 'B':
		return 1;
		break;
	case 'c':
	case 'C':
		return 2;
		break;
	case 'x':
	case 'X':
		return 3;
		break;
	case 'y':
	case 'Y':
		return 4;
		break;
	case 'z':
	case 'Z':
		return 5;
		break;
	case 'i':
	case 'I':
		return 6;
		break;
	case 'j':
	case 'J':
		return 7;
		break;
	default:
		return -1;
		break;
	}
}

// Get argument value for string
argumentStruct_t Assembler::argumentFor(const std::string& arg, bool isB)
{
	argumentStruct_t toReturn;

	toReturn.badArgument = false;
	toReturn.labelReference = "";

	if (arg == "") {
		std::cout << "ERROR: Empty argument string" << std::endl;

		toReturn.badArgument = true;
		return toReturn;
	}

	// If it begins with 0-9 it's a number
	if ((arg[0] >= '0' && arg[0] <= '9') || arg[0] == '-') {
		int argValue;

		std::stringstream ss;

		if (arg.find("0x") != std::string::npos) {
			ss << std::hex << arg;
		}
		else {
			ss << std::dec << arg;
		}

		ss >> argValue;

		if (argValue == 0xffff || (argValue > -1 && argValue < ARG_LITERAL_START && !isB)) {
			toReturn.argument = ARG_LITERAL_START + (argValue == 0xffff ? 0x00 : (0x01 + argValue));

			return toReturn;
		}

		toReturn.argument = ARG_NEXTWORD_LITERAL;
		toReturn.nextWord = argValue;

		return toReturn;
	}

	if (arg[0] == '[' || arg[0] == '(') {
		if (arg.length() == 3 && (arg[2] == ']' || arg[2] == ')')) {
			// If it's 1 char in bracket it's a register
			int regNum = registerFor(arg[1]);

			if (regNum != -1) {
				toReturn.argument = ARG_REG_INDEX_START + regNum;
				return toReturn;
			}
			else {
				std::cout << "ERROR: Invalid [register]: " << arg << std::endl;

				toReturn.badArgument = true;
				return toReturn;
			}
		}

		// TODO: Clean up

		// Hex value?
		int hexValue;
		if (sscanf(arg.c_str() + 1, "0x%x", &hexValue) == 1) {
			// +register?
			char regName;
			if (sscanf(arg.c_str() + 1, "0x%x+%c", &hexValue, &regName) == 2) {
				// TODO enforce closing
				int regNum = registerFor(regName);

				if (regNum != -1) {
					toReturn.argument = ARG_REG_NEXTWORD_INDEX_START + regNum;
					toReturn.nextWord = hexValue;
					return toReturn;
				}
				else {
					std::cout << "ERROR: Invalid register name " << regName << " in: " << arg << std::endl;

					toReturn.badArgument = true;
					return toReturn;
				}
			}
			else {
				// Just hex in brackets
				// TODO: enforce closing
				toReturn.argument = ARG_NEXTWORD;
				toReturn.nextWord = hexValue;
				return toReturn;
			}
		}
		else {
			std::string temp = arg;


			auto labelRef = false;

			if (temp[0] == '[') {
				labelRef = true;

				temp = temp.substr(1, temp.length() - 2);
			}

			int labelEnd = temp.find("+");
			bool containsRegister = false;

			if (labelEnd != std::string::npos) {
				// Label + register
				std::string label = trim(temp.substr(0, labelEnd));

				if (label.length() == 1) {
					int regNum = registerFor(label[0]);

					toReturn.argument = ARG_REG_NEXTWORD_INDEX_START + regNum;

					int argValue = 0;
					std::stringstream ss;

					temp = trim(temp.substr(labelEnd + 1));

					if (temp.find("0x") != std::string::npos) {
						ss << std::hex << temp;
					}
					else {
						ss << std::dec << temp;
					}

					ss >> argValue;

					toReturn.nextWord = argValue;
				}
				else {
					toReturn.labelReference = label;

					std::string regName = trim(temp.substr(labelEnd + 1, temp.length() - 1));

					int regNum = registerFor(regName[0]);
					if (regNum != -1) {
						toReturn.argument = ARG_REG_NEXTWORD_INDEX_START + regNum;
						toReturn.argAlreadySet = true;
					}
					else {
						std::cout << "ERROR: Invalid register name '" << regName << "' in: " << temp << " (" << labelEnd << ")" << std::endl;

						toReturn.badArgument = true;

					}
				}
			}
			else {

				if (labelRef) {
					// Label reference
					toReturn.labelReference = temp;
					toReturn.argument = ARG_NEXTWORD;
					toReturn.argAlreadySet = true;
				}
				else {
					// Regular label
					toReturn.labelReference = temp;
				}
			}

			return toReturn;
		}
	}

	auto reserved = toLower(arg);

	// Check for reserved words
	if (reserved == "pop") {
		toReturn.argument = ARG_PUSH_POP;
		return toReturn;
	}

	if (reserved == "peek") {
		toReturn.argument = ARG_PEEK;
		return toReturn;
	}

	if (reserved == "push") {
		toReturn.argument = ARG_PUSH_POP;
		return toReturn;
	}

	if (reserved == "sp") {
		toReturn.argument = ARG_SP;
		return toReturn;
	}

	if (reserved == "pc") {
		toReturn.argument = ARG_PC;
		return toReturn;
	}

	if (reserved == "") {
		toReturn.argument = ARG_O;
		return toReturn;
	}

	// Is register?
	if (reserved.length() == 1) {
		int regNum = registerFor(reserved[0]);
		if (regNum != -1) {
			toReturn.argument = ARG_REG_START + regNum;
			return toReturn;
		}
	}

	auto label = _foundLabels.find(arg);

	if (label != _foundLabels.end()) {
		auto labelPos = label->second;
		if ((labelPos == 0xffff || labelPos < 31) && isB) {
			toReturn.argument = 0x20 + (labelPos == 0xffff ? 0x00 : (0x01 + labelPos));
		}
		else {
			toReturn.argument = ARG_NEXTWORD_LITERAL;
			toReturn.nextWord = labelPos;
		}
	}
	else {
		// Resolve later
		toReturn.argument = ARG_NEXTWORD_LITERAL;
		toReturn.labelReference = arg;

	}

	return toReturn;
}



int Assembler::compile(const std::string& filename)
{
	std::string compiledFilename = replaceStr(filename, "dasm", "bin");

	std::ifstream sourceFile(filename);

	if (!sourceFile.is_open()) {
		std::cout << "ERROR: Could not open " << filename.c_str() << std::endl;
		return -1;
	}

	std::ofstream compiledFile(compiledFilename, std::ios::binary | std::ios::out);

	if (!compiledFile) {
		std::cout << "ERROR: Could not open " << compiledFilename.c_str() << std::endl;
	}

	bool foundComment = false;
	bool finished = false;

	word_t address = 0;

	std::string command;
	std::string label;
	std::string arg1;
	std::string arg2;
	std::string data;

	bool skipTillNextLine = false;

	std::string currentLine;


	while (1) {


		if (skipTillNextLine) {
			skipTillNextLine = false;
		}

		command = "";
		arg1 = "";
		arg2 = "";
		data = "";

		std::getline(sourceFile, currentLine);

		currentLine = replaceStr(currentLine, "\t", " ");

		if (currentLine.find(";") != std::string::npos) {
			// Remove comment at end of string
			currentLine = currentLine.substr(0, currentLine.find(";"));
		}

		currentLine = trim(currentLine);


		// Check if whole line is a blank
		if (currentLine != "" && currentLine[0] != ';') {

			//	std::cout << currentLine << std::endl;

				// Non blank line, start processing

				// Get label if applicable
			if (currentLine[0] == ':') {
				processLine(currentLine, data, label, skipTillNextLine, command, arg1, arg2, true);

				_foundLabels[label] = address;


			}
			else {
				processLine(currentLine, data, label, skipTillNextLine, command, arg1, arg2, false);
			}

			if (!skipTillNextLine) {

				assembledInstruction_t instruction;

				processCommand(command, data, address, label, instruction);

				if (command != "dat") {
					processArg1(command, arg1, address, label, instruction);
					processArg2(command, arg2, address, label, instruction);
				}

				//	std::cout << "\tCommand: " << command << " Arg1: " << arg1 << " Arg2: " << arg2 << " Dat: " << data << std::endl;


				if (command != "") {

					instruction.source = command;

					if (command == "dat") {
						instruction.source += " " + data;
					}
					else {
						if (arg1 != "") {
							instruction.source += " " + arg1;
						}
						else {
							std::cout << "Expected B arg";
							return false;
						}

						// TODO: Handle cases where arg2 is required.
						if (arg2 != "") {
							instruction.source += ", " + arg2;
						}

					}

					_instructions.emplace_back(instruction);
				}

				else {
					std::cout << "command missing";
				}
			}
		}

		if (sourceFile.eof()) {
			break;
		}

	}


	for (auto& instruction : _instructions) {

		//	std::cout << "Assembling for address " << instruction->address << std::endl;

		if (instruction.data.size() > 0) {
			continue;
		}

		// Label reference for A
		if (instruction.a.labelReference != "") {
			//std::cout << "Unresolved label for a: " << instruction->a.labelReference << std::endl;

			for (auto& other : _instructions) {
				if (other.label != "" && (other.label == instruction.a.labelReference)) {
					// Match
				//	std::cout << "Resolved " << instruction->a.labelReference << " to address " << other->address << std::endl;
					instruction.a.nextWord = other.address;
					instruction.a.labelReference = "";
					break;
				}
			}
		}

		// Label reference for B
		if (instruction.b.labelReference != "") {
			//	std::cout << "Unresolved label for b: " << instruction->b.labelReference << std::endl;

		//	for (AssembledInstructionPtr other = head; other != nullptr; other = other->next) {
			for (auto& other : _instructions) {
				if (other.label != "" && (other.label == instruction.b.labelReference)) {
					// Match
				//	std::cout << "Resolved " << instruction->b.labelReference << " to address " << other->address << std::endl;


					if (!instruction.b.argAlreadySet) {
						instruction.b.argument = ARG_NEXTWORD_LITERAL;

					}

					instruction.b.nextWord = other.address;

					instruction.b.labelReference = "";

					break;
				}
			}
		}

		// Any references left?
		if (instruction.a.labelReference != "") {
			std::cout << "Unresolved label for a: " << instruction.a.labelReference << std::endl;
			return -1;
		}

		if (instruction.b.labelReference != "") {
			std::cout << "Unresolved label for b: " << instruction.b.labelReference << std::endl;
			return -1;
		}
	}

	std::cout << std::endl;

	// Write out code
	//for (AssembledInstructionPtr instruction = head; instruction != nullptr; instruction = instruction->next) {
	for (auto& instruction : _instructions) {


		if (instruction.data.size() > 0) {
			//std::cout << "DATA: " << instruction.dataLength << " words" << std::endl;
			for (int i = 0; i < instruction.data.size(); ++i) {
				compiledFile.write(reinterpret_cast<const char*>(&instruction.data[i]), sizeof word_t);
			}

			std::cout << string_format("%s", instruction.source.c_str()) << std::endl;

			continue;
		}


		instruction_t packed = 0;

		packed = (instruction.b.argument << 10) | (instruction.a.argument << 5) | instruction.opcode;

		// Save instruction
		//std::cout << address << ": Assembled instruction: " << packed << std::endl;
		compiledFile.write(reinterpret_cast<const char*>(&packed), sizeof instruction_t);

		std::string strFormat = "%-50s %-4x ";

		bool includeA = false;
		bool includeB = false;

		if (Cpu::usesNextWord(instruction.b.argument)) {
			//	std::cout << ++address << ": Extra Word B: " << instruction.b.nextWord << std::endl;
			compiledFile.write(reinterpret_cast<const char*>(&instruction.b.nextWord), sizeof word_t);

			strFormat += "%-4x ";

			includeB = true;
		}
		else {
			strFormat += "     ";
		}

		if (instruction.opcode != OP_NONBASIC && Cpu::usesNextWord(instruction.a.argument)) {
			//	std::cout << ++address << ": Extra Word A: " << instruction.a.nextWord << std::endl;
			compiledFile.write(reinterpret_cast<const char*>(&instruction.a.nextWord), sizeof word_t);

			strFormat += "%-4x ";

			includeA = true;
		}
		else {
			strFormat += "     ";
		}


		std::string sourceLine;

		if (!includeA && includeB) {
			sourceLine = string_format(strFormat, instruction.source.c_str(), packed, instruction.b.nextWord);
		}
		else if (includeA && !includeB) {
			sourceLine = string_format(strFormat, instruction.source.c_str(), packed, instruction.a.nextWord);
		}

		else {
			sourceLine = string_format(strFormat, instruction.source.c_str(), packed, instruction.b.nextWord, instruction.a.nextWord);
		}

		std::cout << sourceLine << std::endl;

		_lineMappings.insert(std::pair<word_t, std::string>(instruction.address, sourceLine));


	}

	std::cout << "Program compiled successfully." << std::endl;

	compiledFile.close();
}

// Split up the line and work out what values are in it.
// This is sort of shit, will need to update this at some point.
int Assembler::processLine(const std::string& currentLine, std::string& data, std::string& label, bool &functionOnNextLine,
	std::string& command, std::string& arg1, std::string& arg2, bool containsLabel)
{
	std::string processedLine = currentLine;

	//std::vector<std::string> splitStr;

	//split(currentLine, splitStr, ' ');

	//splitStr.erase(std::remove(splitStr.begin(), splitStr.end(), " "), splitStr.end());

	size_t index;

	if (processedLine[0] == ':') {
		// Don't include ':' in label

		index = processedLine.find(" ");

		label = trim(processedLine.substr(1, index));

		if (index == std::string::npos) {
			functionOnNextLine = true;
			return 1;
		}
		else {
			processedLine = trim(processedLine.substr(index));
		}
		//label = trim(replace(splitStr[0], ',', ' '));
		//label = replaceStr(label, ":", "");

		/*
		if (splitStr.size() == 1) {
			functionOnNextLine = true;
			return 1;
		}
		else if (splitStr.at(1).find(";") != std::string::npos) {
			functionOnNextLine = true;
			return 1;
		}
		*/

	}


	int offset = (containsLabel ? 1 : 0);

	/*
	if (splitStr.size() > 1) {
		command = trim(splitStr[offset]);

		command = toLower(command);

		command = replaceStr(command, ",", "");
	}
	else {
		command = "";
	}
	*/

	command = toLower(processedLine.substr(0, 3));

	processedLine = trim(processedLine.substr(processedLine.find(" ")));

	// Check if remaining data belongs to 'dat' command.
	//int i = strcmp(command, "dat");
	//if (strcmp(command, "dat") == 0) {
	if (command == "dat") {

		/*
		std::string temp = currentLine;

		if (containsLabel) {
			temp = replaceStr(temp, ":" + label, "");

		}

		if (temp.find(";") != std::string::npos) {
			temp = temp.substr(0, temp.find(";"));
		}

		data = trim(replaceStr(temp, "dat", ""));
		*/

		if (processedLine.find(";") != std::string::npos) {
			processedLine = processedLine.substr(0, processedLine.find(";"));
		}

		data = trim(processedLine);

		std::cout << "";
	}
	else {

		auto index = processedLine.find(",");

		if (index != std::string::npos) {
			arg1 = trim(processedLine.substr(0, index));
			arg2 = trim(processedLine.substr(index + 1));

		}
		else {
			arg1 = trim(processedLine);
		}

		/*
		arg1 = trim(replace(splitStr[offset + 1], ',', ' '));


		if (splitStr.size() > (offset + 2)) {
			if (splitStr.at(offset + 2).find(";") == std::string::npos) {
				arg2 = trim(replaceStr(replaceStr(splitStr[offset + 2], ",", ""), "\t", ""));
			}
		}
		*/

	}

	return 0;
}

// Process the command
int Assembler::processCommand(const std::string& command, std::string data, word_t &address, const std::string& label,
	assembledInstruction_t& instruction)
{
	int i = 0, index = 0;

	//instruction = std::make_shared<assembledInstruction_t>();

	/*
	if (head == nullptr) {
		head = instruction;
		tail = instruction;
	}
	else {
		tail->next = instruction;
		tail = tail->next;
	}
	*/

	//instruction->next = nullptr;
	instruction.address = address;
	instruction.label = label;

	//instruction->data = nullptr;

	if (command == "dat") {
		//	instruction->data = (word_t*)malloc(MAX_CHARS * sizeof(word_t));

			/*
			while (1) {

				int nextChar = data[index++];
				if (nextChar == '"') {
					//std::cout << "Reading string." << std::endl;

					bool_t escaped = 0;
					while (1) {
						nextChar = data[index++];
						char toPut;

						if (escaped) {
							// Escape translation
							switch (nextChar) {
							case 'n':
								toPut = '\n';
								break;

							case 't':
								toPut = '\t';
								break;

							case '\\':
								toPut = '\\';
								break;

							case '"':
								toPut = '"';
								break;

							default:
								std::cout << "ERROR: Unrecognized escape sequence " << nextChar << std::endl;
								return -1;
							}

							escaped = 0;
						}
						else if (nextChar == '"' || nextChar == '\0') {
							break;
						}
						else if (nextChar == '\\') {
							escaped = 1;
							continue;
						}
						else {
							// Normal character
							toPut = nextChar;
						}

						instruction->data.push_back(toPut);

					//	instruction->data[instruction->dataLength++] = toPut;
						std::cout << toPut;
					}

					std::cout << std::endl;
				}
				else {
					if (index >= data.length()) {
						break;
					}

					int nextNextChar = data[index++];

					if (nextNextChar == -1) {
						break;
					}

					if (nextChar == '0' && nextNextChar == 'x') {
						// Revert back 2 chars.
						data[index - 2];

						// Hex literal
						std::cout << "Reading hex literal" << std::endl;

						//if (!sscanf(data.c_str(), "0x%hx", &instruction->data[instruction->data.size()]) == 1) {
						//	std::cout << "ERROR: Expected hex literal" << std::endl;						return -1;
						//}

					//	instruction->dataLength++;
					}
					//else if (sscanf(data.c_str(), "%hu", &instruction->data[instruction->data.size()]) == 1) {
						// Decimal literal
					//	std::cout << "Reading decimal literal" << std::endl;
					//	instruction->dataLength++;
					//}
					else {
						// Not a real literal
						std::cout << "Out of literals" << std::endl;
						break;
					}

				}

			}
			*/

		std::vector<std::string> splitDat;

		split(data, splitDat, ',');

		for (auto& dat : splitDat) {
			std::string temp = trim(dat);

			temp = replaceStr(temp, ",", "");

			if (temp.find("\"") != std::string::npos) {
				// Parse string
				temp = replaceStr(temp, "\"", "");

				for (auto& ch : temp) {

					instruction.data.push_back(ch);
				}
			}
			else {
				int datValue;

				std::stringstream ss;

				if (temp.find("0x") != std::string::npos) {
					ss << std::hex << temp;
				}
				else {
					ss << std::dec << temp;
				}

				ss >> datValue;

				instruction.data.push_back(datValue);
			}


		}

		address += instruction.data.size();
	}

	return 1;
}

// Process argument 1
void Assembler::processArg1(const std::string& command, const std::string& arg, word_t& address, const std::string& label,
	assembledInstruction_t& instruction)
{


	// Determine opcode
	instruction.opcode = opcodeFor(command);


	// TODO: Change to b
	instruction.a = argumentFor(arg, true);

	// Advance address
	address++;

	if (Cpu::usesNextWord(instruction.a.argument)) {
		address++;
	}
}

// Process argument 2
void Assembler::processArg2(const std::string& command, const std::string& arg, word_t &address, const std::string& label,
	assembledInstruction_t& instruction)
{
	int i = 0;

	bool preserveArg = false;
	//char tempArg[MAX_CHARS], preservedArg[MAX_CHARS];
	int j = 0, temp = 0;


	if (arg == "") {
		// No second arg
		instruction.b = instruction.a;

		instruction.a.argument = static_cast<argument_t>(nonbasicOpcodeFor(command));
		instruction.a.labelReference = "";

	}
	else {


		// TODO: Change to a
		instruction.b = argumentFor(arg, false);

		if (Cpu::usesNextWord(instruction.b.argument)) {
			address++;
		}
	}
}

std::map<word_t, std::string> Assembler::getLineMappings() const
{
	return _lineMappings;
}
