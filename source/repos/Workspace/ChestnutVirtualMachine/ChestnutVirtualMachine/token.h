#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

class Token {
private:
public:
	Token(std::string identifier) : identifier(identifier) {}
	std::string identifier;
};

enum class special_literal {
	line_break, none,
};

std::vector<Token*> extract_tokens(std::string& str);
special_literal check_special_literal(const std::string& str, int i);