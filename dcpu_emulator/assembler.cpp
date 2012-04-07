#include "StdAfx.h"
#include "assembler.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

Assembler::Assembler(void)
{
}


Assembler::~Assembler(void)
{
}

opcode_t Assembler::opcodeFor(char* command)
{
	if (!strcmp(command, "set")) {
		return OP_SET;
	}

	if (!strcmp(command, "add")) {
		return OP_ADD;
	}

	if (!strcmp(command, "sub")) {
		return OP_SUB;
	}

	if (!strcmp(command, "mul")) {
		return OP_MUL;
	}

	if (!strcmp(command, "div")) {
		return OP_DIV;
	}

	if (!strcmp(command, "mod")) {
		return OP_MOD;
	}

	if (!strcmp(command, "shl")) {
		return OP_SHL;
	}

	if (!strcmp(command, "shr")) {
		return OP_SHR;
	}

	if (!strcmp(command, "and")) {
		return OP_AND;
	}

	if (!strcmp(command, "bor")) {
		return OP_BOR;
	}

	if (!strcmp(command, "xor")) {
		return OP_XOR;
	}

	if (!strcmp(command, "ife")) {
		return OP_IFE;
	}

	if (!strcmp(command, "ifn")) {
		return OP_IFN;
	}

	if (!strcmp(command, "ifg")) {
		return OP_IFG;
	}
	if (!strcmp(command, "ifb")) {
		return OP_IFB;
	}

	// Assume non-basic
	return OP_NONBASIC;
}

// Get non-basic opcode from string
nonbasicOpcode_t Assembler::nonbasicOpcodeFor(char* command)
{
	if (!strcmp(command, "jsr")) {
		return OP_JSR;
	}

	// Instruction not found
	std::cout << "ERROR: Unkown instruction \"" << command << "\"" << std::endl;
	return 0;
}

// Check register number 
int Assembler::registerFor(char regName)
{
	switch(regName) {
	case 'a':
		return 0;
		break;
	case 'b':
		return 1;
		break;
	case 'c':
		return 2;
		break;
	case 'x':
		return 3;
		break;
	case 'y':
		return 4;
		break;
	case 'z':
		return 5;
		break;
	case 'i':
		return 6;
		break;
	case 'j':
		return 7;
		break;
	default:
		return -1;
		break;
	}
}

// Get argument value for string
argumentStruct_t Assembler::argumentFor(char* arg)
{
	argumentStruct_t toReturn;

	toReturn.badArgument = false;
	toReturn.labelReference = NULL;

	if (strlen(arg) == 0) {
		std::cout << "ERROR: Empty argument string" << std::endl;

		toReturn.badArgument = true;
		return toReturn;
	}

	// If it begins with 0-9 it's a number
	if (arg[0] >= '0' && arg[0] <= '9') {
		int argValue;
		char* format;

		if (strlen(arg) > 2 && arg[0] == '0' && arg[1] == 'x') {
			// Value is hex
			format = "%x";
		} else {
			// Decimal
			format = "%d";
		}

		if (sscanf(arg, format, &argValue) != 1) {
			std::cout << "ERROR: Invalid literal value: " << arg << std::endl;

			toReturn.badArgument = true;
			return toReturn;
		}

		if (argValue < ARG_LITERAL_END - ARG_LITERAL_START) {
			toReturn.argument = ARG_LITERAL_START + argValue;

			return toReturn;
		}

		toReturn.argument = ARG_NEXTWORD;
		toReturn.nextWord = argValue;

		return toReturn;
	}

	if (arg[0] == '[' || arg[0] == '(') {
		if (strlen(arg) == 3 && (arg[2] == ']' || arg[2] == ')')) {
			// If it's 1 char in bracket it's a register
			int regNum = registerFor(arg[1]);

			if (regNum != -1) {
				toReturn.argument = ARG_REG_INDEX_START + regNum;
				return toReturn;
			} else {
				std::cout << "ERROR: Invalid [register]: " << arg << std::endl;

				toReturn.badArgument = true;
				return toReturn;
			}
		}

		// Hex value?
		int hexValue;
		if (sscanf(arg + 1, "0x%x", &hexValue) == 1) {
			// +register?
			char regName;
			if (sscanf(arg + 1, "0x%x+%c", &hexValue, &regName) == 2) {
				// TODO enforce closing
				int regNum = registerFor(regName);

				if (regNum != -1) {
					toReturn.argument = ARG_REG_NEXTWORD_INDEX_START + regNum;
					toReturn.nextWord = hexValue;
					return toReturn;
				} else {
					std::cout << "ERROR: Invalid register name " << regName << " in: " << arg << std::endl;

					toReturn.badArgument = true;
					return toReturn;
				}
			} else {
				// Just hex in brackets
				// TODO: enforce closing
				toReturn.argument = ARG_NEXTWORD_INDEX;
				toReturn.nextWord = hexValue;
				return toReturn;
			}
		} else {
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

			// Store lable
			char* label = (char*) malloc((labelEnd - labelStart) + 1);
			strncpy(label, labelStart, (labelEnd - labelStart));
			label[labelEnd - labelStart] = '\0';

			toReturn.labelReference = label;

			// Try to parse register
			char regName;
			if (sscanf(labelEnd, "+%c", &regName) == 1) {
				int regNum = registerFor(regName);
				if (regNum != -1) {
					toReturn.argument = ARG_REG_NEXTWORD_INDEX_START + regNum;
					return toReturn;
				} else {
					std::cout << "ERROR: Invalid register name '" << regName << "' in: " << arg << " (" << labelEnd << ")" << std::endl;

					toReturn.badArgument = true;
					return toReturn;
				}
			} else {
				toReturn.argument = ARG_NEXTWORD_INDEX;
				return toReturn;
			}
		}
	}
	// Check for reserved words
	if (!strcmp(arg, "pop")) {
		toReturn.argument = ARG_POP;
		return toReturn;
	}

	if (!strcmp(arg, "peek")) {
		toReturn.argument = ARG_PEEK;
		return toReturn;
	}

	if (!strcmp(arg, "push")) {
		toReturn.argument = ARG_PUSH;
		return toReturn;
	}

	if (!strcmp(arg, "sp")) {
		toReturn.argument = ARG_SP;
		return toReturn;
	}

	if (!strcmp(arg, "pc")) {
		toReturn.argument = ARG_PC;
		return toReturn;
	}

	if (!strcmp(arg, "o")) {
		toReturn.argument = ARG_O;
		return toReturn;
	}

	// Is register?
	if (strlen(arg) == 1) {
		int regNum = registerFor(arg[0]);
		if (regNum != -1) {
			toReturn.argument = ARG_REG_START + regNum;
			return toReturn;
		}
	}

	toReturn.argument = ARG_NEXTWORD;

	// Store label for later
	char* label = (char*) malloc(strlen(arg) + 1);
	strcpy(label, arg);

	toReturn.labelReference = label;
	return toReturn;
}

int Assembler::compile(char* filename)
{
	char* compiledFilename = "demo2.obj";

	FILE* sourceFile = fopen(filename, "r");

	if (!sourceFile) {
		std::cout << "ERROR: Could not open " << filename << std::endl;
	}

	// TODO: Add automatic file naming
	FILE* compiledFile = fopen(compiledFilename, "w");

	if (!compiledFile) {
		std::cout << "ERROR: Could not open " << filename << std::endl;
	}

	word_t address = 0;

	assembledInstruction_t* head = NULL;
	assembledInstruction_t* tail = NULL;

	while (1) {
		fscanf(sourceFile, " ");		// Consume whitespace

		int nextChar = fgetc(sourceFile);
		if (nextChar == ';') {
			// Comment, ignore rest of line
			while (nextChar != EOF && nextChar != '\n') {
				nextChar = fgetc(sourceFile);
			}
			continue;
		} else {
			ungetc(nextChar, sourceFile);
		}

		char* label;
		char command[MAX_CHARS], arg1[MAX_CHARS], arg2[MAX_CHARS];

		// Find labels
		label = (char*) malloc(MAX_CHARS);
		if (fscanf(sourceFile, ":%s ", label) == 1) {
			// Make it lowercase
			int i = 0;
			while (label[i] != '\0') {
				label[i] = tolower(label[i]);
				i++;
			}
		} else {
			free(label);
			label = NULL;
		}

		// Read the command
		if (fscanf(sourceFile, "%s", command) == 1) {
			// Add new instruction
			//assembledInstruction_t* instruction = (assembledInstruction_t*) malloc(sizeof(assembledInstruction_t));
			assembledInstruction_t* instruction = new assembledInstruction_t;
			if (head == NULL) {
				head = instruction;
				tail = instruction;
			} else {
				tail->next = instruction;
				tail = tail->next;
			}

			instruction->next = NULL;
			instruction->address = address;
			instruction->label = label;
			instruction->data = NULL;

			int i = 0;
			while (command[i] != '\0') {
				command[i] = tolower(command[i]);
				i++;
			}

			if (!strcmp(command, "dat")) {
				instruction->data = (word_t*) malloc(MAX_CHARS * sizeof(word_t));
				instruction->dataLength = 0;

				while(1) {
					fscanf(sourceFile, " ");

					int nextChar = fgetc(sourceFile);
					if (nextChar = '"') {
						std::cout << "Reading string." << std::endl;

						bool_t escaped = 0;
						while(1) {
							nextChar = fgetc(sourceFile);
							char toPut;

							if (escaped) {
								// Escape translation
								switch(nextChar) {
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
							} else if (nextChar == '"') {
								break;
							} else if (nextChar == '\\') {
								escaped = 1;
								continue;
							} else {
								// Normal character
								toPut = nextChar;
							}

							instruction->data[instruction->dataLength++] = toPut;
							std::cout << toPut;
						}

						std::cout << std::endl;
					} else {
						int nextNextChar = fgetc(sourceFile);

						// Unget last 2 characters
						fseek(sourceFile, -2, SEEK_CUR);

						if (nextChar == '0' && nextNextChar == 'x') {
							// Hex literal
							std::cout << "Reading hex literal" << std::endl;

							if (!fscanf(sourceFile, "0x%hx", &instruction->data[instruction->dataLength]) == 1) {
								std::cout << "ERROR: Expected hex literal" << std::endl;
								return -1;
							}

							instruction->dataLength++;
						} else if(fscanf(sourceFile, "%hu", &instruction->data[instruction->dataLength]) ==1) {
							// Decimal literal
							std::cout << "Reading decimal literal" << std::endl;
							instruction->dataLength++;
						} else {
							// Not a real literal
							std::cout << "Out of literals" << std::endl;
							break;
						}
					}

					// Consume comma
					fscanf(sourceFile, ",");
				}

				address += instruction->dataLength;
			} else if (fscanf(sourceFile, " %s", arg1) == 1) {
				i = 0;

				while (arg1[i] != '\0') {
					if (arg1[i] == ',' && arg1[i + 1] == '\0') {
						arg1[i] = '\0';
						continue;
					}

					arg1[i] = tolower(arg1[i]);
					i++;
				}

				std::cout << "Argument 1: " << arg1 << std::endl;

				// Determine opcode
				instruction->opcode = opcodeFor(command);
				//std::cout << "Basic opcode: " << instruction->opcode << std::endl;
				printf("Basic opcode: %d\n", instruction->opcode);

				instruction->a = argumentFor(arg1);

				// Advance address
				address++;

				if (Cpu::usesNextWord(instruction->a.argument)) {
					address++;
				}

				if (instruction->opcode == OP_NONBASIC) {
					// No second argument
					instruction->b = instruction->a;

					instruction->a.argument = (argument_t) nonbasicOpcodeFor(command);
					instruction->a.labelReference = NULL;

					std::cout << "Non-basic opcode: " << instruction->a.argument << std::endl;
				} else {
					// Second argument
					if (fscanf(sourceFile, "%s", arg2) != 1) {
						std::cout << " ERROR: Missing second argument for " << command << " (got " << arg2 << ")" << std::endl;
						return -1;
					}

					i = 0;
					while (arg2[i] != '\0') {
						arg2[i] = tolower(arg2[i]);
						i++;
					}

					instruction->b = argumentFor(arg2);

					if (Cpu::usesNextWord(instruction->b.argument)) {
						address++;
					}
				}
			}
		} else {
			std::cout << "No more valid codes" << std::endl;

			fclose(sourceFile);

			// Assemble binary code
			for (assembledInstruction_t* instruction = head; instruction != NULL; instruction = instruction->next) {
				std::cout << "Assembling for address " << instruction->address << std::endl;

				if (instruction->data != NULL) {
					continue;
				}

				// Label reference for A
				if (instruction->a.labelReference != NULL) {
					std::cout << "Unresolved label for a: " << instruction->a.labelReference << std::endl;

					for (assembledInstruction_t* other = head; other != NULL; other = other->next) {
						if (other->label != NULL && !strcmp(other->label, instruction->a.labelReference)) {
							// Match
							std::cout << "Resolved " << instruction->a.labelReference << " to address " << other->address << std::endl;
							instruction->a.nextWord = other->address;
							instruction->a.labelReference = NULL;
							break;
						}
					}
				}

				// Label reference for B
				if (instruction->b.labelReference != NULL) {
					std::cout << "Unresolved label for b: " << instruction->b.labelReference << std::endl;

					for (assembledInstruction_t* other = head; other != NULL; other = other->next) {
						if (other->label != NULL && !strcmp(other->label, instruction->b.labelReference)) {
							// Match
							std::cout << "Resolved " << instruction->b.labelReference << " to address " << other->address << std::endl;
							instruction->b.nextWord = other->address;
							instruction->b.labelReference = NULL;
							break;
						}
					}
				}

				// Any references left?
				if (instruction->a.labelReference != NULL) {
					std::cout << "Unresolved label for a: " << instruction->b.labelReference << std::endl;
					return -1;
				}

				if (instruction->b.labelReference != NULL) {
					std::cout << "Unresolved label for b: " << instruction->b.labelReference << std::endl;
					return -1;
				}
			}

			// Write out code
			for (assembledInstruction_t* instruction = head; instruction != NULL; instruction = instruction->next) {
				if (instruction->data != NULL) {
					std::cout << "DATA: " << instruction->dataLength << " words" << std::endl;
					fwrite(instruction->data, sizeof(word_t), instruction->dataLength, compiledFile);
					continue;
				}

				instruction_t packed = 0;
				packed = Cpu::setOpcode(packed, instruction->opcode);
				packed = Cpu::setArgument(packed, 0, instruction->a.argument);
				packed = Cpu::setArgument(packed, 1, instruction->b.argument);

				// Save instruction
				std::cout << address << ": Assembled instruction: " << packed << std::endl;
				fwrite(&packed, sizeof(instruction_t), 1, compiledFile);

				if (instruction->opcode != OP_NONBASIC && Cpu::usesNextWord(instruction->a.argument)) {
					std::cout << ++address << ": Extra Word A: " << instruction->a.nextWord << std::endl;
					fwrite(&(instruction->a.nextWord), sizeof(word_t), 1, compiledFile);
				}

				if (Cpu::usesNextWord(instruction->b.argument)) {
					std::cout << ++address << ": Extra Word B: " << instruction->b.nextWord << std::endl;
					fwrite(&(instruction->b.nextWord), sizeof(word_t), 1, compiledFile);
				}
			}

			std::cout << "Program compiled successfully." << std::endl;

			fclose(compiledFile);

			break;
		}
	} 
}
