#pragma once

using word_t = unsigned short;
using instruction_t = word_t;

using argument_t = unsigned char;
using opcode = unsigned char;
using  nonbasic_opcode = argument_t;

using bool_t = unsigned char;

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args) {
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;

	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

std::string& ltrim(std::string& str);
std::string& rtrim(std::string& str);
std::string& trim(std::string& str);
unsigned int split(const std::string &txt, std::vector<std::string> &strs, char splitChar);
std::string replace(const std::string& input, char from, char to);
std::string replaceStr(const std::string& str, const std::string& from, const std::string& to);
std::string toLower(const std::string& input);