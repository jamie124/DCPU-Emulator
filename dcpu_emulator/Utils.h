#pragma once

using word_t = unsigned short;
using instruction_t = word_t;

using argument_t = unsigned char;
using opcode_t = unsigned char;
using nonbasicOpcode_t = argument_t;

using bool_t = unsigned char;

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args) {
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;

	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
