#include "StdAfx.h"
#include "assembler.h"






Assembler::Assembler(void)
{
}


Assembler::~Assembler(void)
{
}

opcode_t Assembler::opcodeFor(const std::string& command)
{
	if (command == "set") {
		return OP_SET;
	}

	if (command == "add") {
		return OP_ADD;
	}

	if (command == "sub") {
		return OP_SUB;
	}

	if (command == "mul") {
		return OP_MUL;
	}

	if (command == "mli") {
		return OP_MLI;
	}

	if (command == "div") {
		return OP_DIV;
	}

	if (command == "dvi") {
		return OP_DVI;
	}

	if (command == "mod") {
		return OP_MOD;
	}

	if (command == "mdi") {
		return OP_MDI;
	}


	if (command == "and") {
		return OP_AND;
	}

	if (command == "bor") {
		return OP_BOR;
	}

	if (command == "xor") {
		return OP_XOR;
	}

	if (command == "shr") {
		return OP_SHR;
	}

	if (command == "asr") {
		return OP_ASR;
	}

	if (command == "shl") {
		return OP_SHL;
	}

	if (command == "ifb") {
		return OP_IFB;
	}

	if (command == "ifc") {
		return OP_IFC;
	}

	if (command == "ife") {
		return OP_IFE;
	}

	if (command == "ifn") {
		return OP_IFN;
	}

	if (command == "ifg") {
		return OP_IFG;
	}

	if (command == "ifa") {
		return OP_IFA;
	}

	if (command == "ifl") {
		return OP_IFL;
	}

	if (command == "ifu") {
		return OP_IFU;
	}

	if (command == "adx") {
		return OP_ADX;
	}

	if (command == "sbx") {
		return OP_SBX;
	}

	if (command == "sti") {
		return OP_STI;
	}

	if (command == "std") {
		return OP_STD;
	}

	// Assume non-basic
	return OP_NONBASIC;
}

// Get non-basic opcode from string
nonbasicOpcode_t Assembler::nonbasicOpcodeFor(const std::string& command)
{
	//if (!strcmp(command, "jsr")) {
	if (command == "jsr") {
		return OP_JSR;
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

		/*
		char* format;

		if (arg.length() > 2 && arg[0] == '0' && arg[1] == 'x') {
			// Value is hex
			format = "%x";
		}
		else {
			// Decimal
			format = "%d";
		}
		*/

		std::stringstream ss;

		if (arg.find("0x") != std::string::npos) {
			ss << std::hex << arg;
		}
		else {
			ss << std::dec << arg;
		}

		ss >> argValue;


		//if (sscanf(arg, format, &argValue) != 1) {
		//	std::cout << "ERROR: Invalid literal value: " << arg << std::endl;

		//	toReturn.badArgument = true;
		//	return toReturn;
		//}

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
			int labelEnd = arg.find("+");


			bool containsRegister = false;

			if (labelEnd != std::string::npos) {
				// Label + register
				std::string label = arg.substr(1, labelEnd - 1);

				toReturn.labelReference = label;

				std::string regName = arg.substr(labelEnd + 1, arg.length() - 1);

				int regNum = registerFor(regName[0]);
				if (regNum != -1) {
					toReturn.argument = ARG_REG_NEXTWORD_INDEX_START + regNum;
					toReturn.argAlreadySet = true;
				}
				else {
					std::cout << "ERROR: Invalid register name '" << regName << "' in: " << arg << " (" << labelEnd << ")" << std::endl;

					toReturn.badArgument = true;

				}
			}
			else {
				// Label
				toReturn.labelReference = arg;
			}

			return toReturn;

			/*
			char* labelStart = arg + 1;

			char* labelEnd = strchr(arg, '+');
			if (labelEnd == NULL) {
				labelEnd = strchr(arg, ']');
			}

			if (labelEnd == NULL) {
				labelEnd = strchr(arg, ')');
			}

			if (labelEnd == NULL) {
				std::cout << "ERROR: Unterminated label in argument: " << arg << std::endl;

				toReturn.badArgument = true;
				return toReturn;
			}


			// Store label
			char* label = (char*)malloc((labelEnd - labelStart) + 1);
			strncpy(label, labelStart, (labelEnd - labelStart));
			label[labelEnd - labelStart] = '\0';

			toReturn.labelReference = label;
			*/

			// Try to parse register
			if (containsRegister) {
				/*
				char regName;
				if (sscanf(labelEnd, "+%c", &regName) == 1) {
					int regNum = registerFor(regName);
					if (regNum != -1) {
						toReturn.argument = ARG_REG_NEXTWORD_INDEX_START + regNum;
						return toReturn;
					}
					else {
						std::cout << "ERROR: Invalid register name '" << regName << "' in: " << arg << " (" << labelEnd << ")" << std::endl;

						toReturn.badArgument = true;
						return toReturn;
					}
				}
				else {
					toReturn.argument = ARG_NEXTWORD_INDEX;
					return toReturn;
				}
				*/
				//	toReturn.argument = ARG_NEXTWORD_LITERAL;
				//	return toReturn;
			}
			else {
				//	toReturn.argument = ARG_NEXTWORD_LITERAL;
				//	return toReturn;
			}
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

	if (reserved == "o") {
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
		if (labelPos == 0xffff || labelPos < 31) {
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

	//toReturn.labelReference = arg;
	return toReturn;
}

std::string replaceStr(const std::string& str, const std::string& from, const std::string& to) {
	std::string temp = str;

	size_t start_pos = temp.find(from);
	while (start_pos != std::string::npos) {
		temp.replace(start_pos, from.length(), to);

		start_pos = temp.find(from);

	}

	return temp;

	//	return temp.replace(start_pos, from.length(), to);

}

int Assembler::compile(const std::string& filename)
{
	std::string compiledFilename = replaceStr(filename, "dasm16", "bin");

	std::ifstream sourceFile(filename);

	if (!sourceFile.is_open()) {
		std::cout << "ERROR: Could not open " << filename.c_str() << std::endl;
		return -1;
	}

	//FILE* compiledFile = fopen(compiledFilename.c_str(), "w");
	std::ofstream compiledFile(compiledFilename, std::ios::binary | std::ios::out);

	if (!compiledFile) {
		std::cout << "ERROR: Could not open " << compiledFilename.c_str() << std::endl;
	}

	char lineBuffer[MAX_CHARS];

	bool foundComment = false;
	bool finished = false;

	word_t address = 0;

	AssembledInstructionPtr head;
	AssembledInstructionPtr tail;
	AssembledInstructionPtr instruction;

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
		currentLine = trim(currentLine);


		// Check if whole line is a blank
		if (currentLine != "" && currentLine[0] != ';') {

			std::cout << currentLine << std::endl;

			// Non blank line, start processing

			// Get label if applicable
			if (currentLine[0] == ':') {
				processLine(currentLine, data, label, skipTillNextLine, command, arg1, arg2, true);

				_foundLabels[label] = address;

				//		std::cout << "label: " << label << " " << std::endl;

			}
			else {
				processLine(currentLine, data, label, skipTillNextLine, command, arg1, arg2, false);
			}

			if (!skipTillNextLine) {

				processCommand(command, data, address, label, head, tail, instruction);

				if (command != "dat") {
					processArg1(command, arg1, address, label, instruction);
					processArg2(command, arg2, address, label, instruction);
				}

				//	std::cout << "\tCommand: " << command << " Arg1: " << arg1 << " Arg2: " << arg2 << " Dat: " << data << std::endl;
			}

			if (command == "dat") {
				instruction->source = command + " " + data;
			}
			else {
				instruction->source = command + " " + arg1 + ", " + arg2;
			}
		}

		if (sourceFile.eof()) {
			break;
		}

	}

	//	std::cout << std::endl;

	for (AssembledInstructionPtr instruction = head; instruction != nullptr; instruction = instruction->next) {
		std::cout << "Assembling for address " << instruction->address << std::endl;

		if (instruction->data != nullptr) {
			continue;
		}

		// Label reference for A
		if (instruction->a.labelReference != "") {
			std::cout << "Unresolved label for a: " << instruction->a.labelReference << std::endl;

			for (AssembledInstructionPtr other = head; other != nullptr; other = other->next) {
				if (other->label != "" && (other->label == instruction->a.labelReference)) {
					// Match
					std::cout << "Resolved " << instruction->a.labelReference << " to address " << other->address << std::endl;
					instruction->a.nextWord = other->address;
					instruction->a.labelReference = "";
					break;
				}
			}
		}

		// Label reference for B
		if (instruction->b.labelReference != "") {
			std::cout << "Unresolved label for b: " << instruction->b.labelReference << std::endl;

			for (AssembledInstructionPtr other = head; other != nullptr; other = other->next) {
				if (other->label != "" && (other->label == instruction->b.labelReference)) {
					// Match
					std::cout << "Resolved " << instruction->b.labelReference << " to address " << other->address << std::endl;


					if (!instruction->b.argAlreadySet) {
						instruction->b.argument = ARG_NEXTWORD_LITERAL;

					}

					instruction->b.nextWord = other->address;

					instruction->b.labelReference = "";
					break;
				}
			}
		}

		// Any references left?
		if (instruction->a.labelReference != "") {
			std::cout << "Unresolved label for a: " << instruction->a.labelReference << std::endl;
			return -1;
		}

		if (instruction->b.labelReference != "") {
			std::cout << "Unresolved label for b: " << instruction->b.labelReference << std::endl;
			return -1;
		}
	}

	std::cout << std::endl;

	// Write out code
	for (AssembledInstructionPtr instruction = head; instruction != nullptr; instruction = instruction->next) {



		if (instruction->data != nullptr) {
			//std::cout << "DATA: " << instruction->dataLength << " words" << std::endl;
			for (int i = 0; i < instruction->dataLength; ++i) {
				compiledFile.write(reinterpret_cast<const char*>(&instruction->data[i]), sizeof word_t);
			}

			std::cout << string_format("%s", instruction->source.c_str()) << std::endl;

			continue;
		}


		instruction_t packed = 0;

		packed = (instruction->b.argument << 10) | (instruction->a.argument << 5) | instruction->opcode;

		// Save instruction
		//std::cout << address << ": Assembled instruction: " << packed << std::endl;
		compiledFile.write(reinterpret_cast<const char*>(&packed), sizeof instruction_t);

		std::string strFormat = "%-25s %-4x ";

		bool includeA = false;
		bool includeB = false;

		if (Cpu::usesNextWord(instruction->b.argument)) {
			//	std::cout << ++address << ": Extra Word B: " << instruction->b.nextWord << std::endl;
			compiledFile.write(reinterpret_cast<const char*>(&instruction->b.nextWord), sizeof word_t);

			strFormat += "%-4x ";

			includeB = true;
		}
		else {
			strFormat += "     ";
		}

		if (instruction->opcode != OP_NONBASIC && Cpu::usesNextWord(instruction->a.argument)) {
			//	std::cout << ++address << ": Extra Word A: " << instruction->a.nextWord << std::endl;
			compiledFile.write(reinterpret_cast<const char*>(&instruction->a.nextWord), sizeof word_t);

			strFormat += "%-4x ";

			includeA = true;
		}
		else {
			strFormat += "     ";
		}



		if (!includeA && includeB) {
			std::cout << string_format(strFormat, instruction->source.c_str(), packed, instruction->b.nextWord) << std::endl;
		}
		else if (includeA && !includeB) {
			std::cout << string_format(strFormat, instruction->source.c_str(), packed, instruction->a.nextWord) << std::endl;
		}

		else {
			std::cout << string_format(strFormat, instruction->source.c_str(), packed, instruction->b.nextWord, instruction->a.nextWord) << std::endl;
		}


	}

	std::cout << "Program compiled successfully." << std::endl;

	compiledFile.close();
}

// Remove any extra characters to make line easier to parse
char* Assembler::cleanString(char *rawLine)
{
	char temp[MAX_CHARS];
	int tempIndex = 0, rawIndex = 0;

	if (rawLine[0] != ';') {

		bool removingDuplicates = false;
		bool skippingChars = false;

		if (rawLine[0] == ' ' || rawLine[0] == '\t') {
			skippingChars = true;
		}

		while (rawLine[rawIndex] != ';' && rawLine[rawIndex] != '\0') {
			if (rawLine[rawIndex] >= 65 && rawLine[rawIndex] < 123
				|| rawLine[rawIndex] == ':') {
				skippingChars = false;
			}

			if (!skippingChars) {
				if (rawLine[rawIndex] == '\t') {
					// Replace tab with space
					temp[tempIndex++] = ' ';
				}
				else {
					temp[tempIndex++] = rawLine[rawIndex];
				}
			}

			rawIndex++;
		}

	}
	temp[tempIndex] = '\0';

	return temp;
}

std::string& Assembler::ltrim(std::string& str)
{
	auto it2 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
	str.erase(str.begin(), it2);
	return str;
}

std::string& Assembler::rtrim(std::string& str)
{
	auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
	str.erase(it1.base(), str.end());
	return str;
}

std::string& Assembler::trim(std::string& str)
{
	return ltrim(rtrim(str));
}

unsigned int Assembler::split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
	auto pos = txt.find(ch);
	auto initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}

std::string Assembler::replace(const std::string& input, char from, char to)
{
	std::string s = input;

	std::replace(s.begin(), s.end(), from, to);

	return s;
}

std::string Assembler::toLower(const std::string& input)
{
	auto arg = input;

	std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);

	return arg;
}

// Split up the line and work out what values are in it.
// This is sort of shit, will need to update this at some point.
int Assembler::processLine(const std::string& currentLine, std::string& data, std::string& label, bool &functionOnNextLine,
	std::string& command, std::string& arg1, std::string& arg2, bool containsLabel)
{

	std::vector<std::string> splitStr;

	split(currentLine, splitStr, ' ');

	splitStr.erase(std::remove(splitStr.begin(), splitStr.end(), " "), splitStr.end());

	//int lineIndex = 0;						// Current position in line
//	int itemIndex = 0;						// Current position in item being stored

	if (containsLabel) {
		// Don't include ':' in label

		label = trim(replace(splitStr[0], ',', ' '));
		label = replaceStr(label, ":", "");

		if (splitStr.size() == 1) {
			functionOnNextLine = true;
			return 1;
		}
		else if (splitStr.at(1).find(";") != std::string::npos) {
			functionOnNextLine = true;
			return 1;
		}

	}


	int offset = (containsLabel ? 1 : 0);

	if (splitStr.size() > 1) {
		command = trim(splitStr[offset]);

		command = toLower(command);
	}
	else {
		command = "";
	}


	// Check if remaining data belongs to 'dat' command.
	//int i = strcmp(command, "dat");
	//if (strcmp(command, "dat") == 0) {
	if (command == "dat") {

		std::string temp = currentLine;

		if (containsLabel) {
			temp = replaceStr(temp, ":" + label, "");

		}

		if (temp.find(";") != std::string::npos) {
			temp = temp.substr(0, temp.find(";"));
		}

		data = trim(replaceStr(temp, "dat", ""));


		std::cout << "";
	}
	else {

		arg1 = trim(replace(splitStr[offset + 1], ',', ' '));


		if (splitStr.size() > (offset + 1) && splitStr.at(offset + 2).find(";") == std::string::npos) {

			arg2 = trim(replaceStr(replaceStr(splitStr[offset + 2], ",", ""), "\t", ""));
		}

	}

	return 0;
}

// Process the command
int Assembler::processCommand(const std::string& command, std::string data, word_t &address, const std::string& label,
	AssembledInstructionPtr &head, AssembledInstructionPtr &tail, AssembledInstructionPtr &instruction)
{
	int i = 0, index = 0;

	//	while (command[i] != '\0') {
	//		command[i] = tolower(command[i]);
	//		i++;
	//	}

	instruction = std::make_shared<assembledInstruction_t>();
	if (head == nullptr) {
		head = instruction;
		tail = instruction;
	}
	else {
		tail->next = instruction;
		tail = tail->next;
	}

	instruction->next = nullptr;
	instruction->address = address;
	instruction->label = label;

	instruction->data = nullptr;

	//if (!strcmp(command, "dat")) {
	if (command == "dat") {
		instruction->data = (word_t*)malloc(MAX_CHARS * sizeof(word_t));
		instruction->dataLength = 0;

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

					instruction->data[instruction->dataLength++] = toPut;
					std::cout << toPut;
				}

				std::cout << std::endl;
			}
			else {
				int nextNextChar = data[index++];

				if (nextNextChar == -1) {
					break;
				}

				if (nextChar == '0' && nextNextChar == 'x') {
					// Revert back 2 chars.
					data[index - 2];

					// Hex literal
					std::cout << "Reading hex literal" << std::endl;

					if (!sscanf(data.c_str(), "0x%hx", &instruction->data[instruction->dataLength]) == 1) {
						std::cout << "ERROR: Expected hex literal" << std::endl;						return -1;
					}

					instruction->dataLength++;
				}
				else if (sscanf(data.c_str(), "%hu", &instruction->data[instruction->dataLength]) == 1) {
					// Decimal literal
					std::cout << "Reading decimal literal" << std::endl;
					instruction->dataLength++;
				}
				else {
					// Not a real literal
					std::cout << "Out of literals" << std::endl;
					break;
				}

			}

		}

		address += instruction->dataLength;
	}

	return 1;
}

// Process argument 1
void Assembler::processArg1(const std::string& command, const std::string& arg, word_t &address, const std::string& label,
	AssembledInstructionPtr &instruction)
{


	// Determine opcode
	instruction->opcode = opcodeFor(command);


	// TODO: Change to b
	instruction->a = argumentFor(arg, true);

	// Advance address
	address++;

	if (Cpu::usesNextWord(instruction->a.argument)) {
		address++;
	}
}

// Process argument 2
void Assembler::processArg2(const std::string& command, const std::string& arg, word_t &address, const std::string& label,
	AssembledInstructionPtr &instruction)
{
	int i = 0;

	bool preserveArg = false;
	char tempArg[MAX_CHARS], preservedArg[MAX_CHARS];
	int j = 0, temp = 0;


	if (arg == "") {
		// No second arg
		instruction->b = instruction->a;

		instruction->a.argument = (argument_t)nonbasicOpcodeFor(command);
		instruction->a.labelReference = "";

	}
	else {


		// TODO: Change to a
		instruction->b = argumentFor(arg, false);

		if (Cpu::usesNextWord(instruction->b.argument)) {
			address++;
		}
	}
}