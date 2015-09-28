#include "StdAfx.h"

std::string& ltrim(std::string& str)
{
	auto it2 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
	str.erase(str.begin(), it2);
	return str;
}

std::string& rtrim(std::string& str)
{
	auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
	str.erase(it1.base(), str.end());
	return str;
}

std::string& trim(std::string& str)
{
	return ltrim(rtrim(str));
}

unsigned int split(const std::string &txt, std::vector<std::string> &strs, char splitChar)
{
	auto pos = txt.find(splitChar);
	auto initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
		initialPos = pos + 1;

		pos = txt.find(splitChar, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}


std::string replace(const std::string& input, char from, char to)
{
	std::string s = input;

	std::replace(s.begin(), s.end(), from, to);

	return s;
}

std::string replaceStr(const std::string& str, const std::string& from, const std::string& to) {
	std::string temp = str;

	size_t start_pos = temp.find(from);
	while (start_pos != std::string::npos) {
		temp.replace(start_pos, from.length(), to);

		start_pos = temp.find(from);

	}

	return temp;

}

std::string toLower(const std::string& input)
{
	auto arg = input;

	std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);

	return arg;
}
