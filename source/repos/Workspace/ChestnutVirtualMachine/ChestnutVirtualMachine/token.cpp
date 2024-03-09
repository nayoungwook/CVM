#include "token.h"

special_literal check_special_literal(const std::string& str, int i) {
	if (str.size() > i + 1) {
		if (str[i + 1] == 'n') {
			return special_literal::line_break;
		}
	}
	return special_literal::none;
}

std::vector<Token*> extract_tokens(std::string& str) {

	std::string tkn = "";
	std::vector<Token*> result;
	bool string_literal = false;

	for (int i = 0; i < str.length(); i++) {
		// string literal
		if (str[i] == '\"') {
			if (!string_literal) {
				string_literal = true;
			}
			else {
				string_literal = false;
				tkn += str[i];
				result.push_back(new Token(tkn));
				tkn = "";
				i++;
				continue;
			}
		}

		// cut token
		if (isspace(str[i]) && !string_literal) {
			if (!(tkn == " " || tkn == "")) {
				result.push_back(new Token(tkn));
			}
			tkn = "";
			continue;
		}

		if (string_literal && str[i] == '\\') {
			special_literal result = check_special_literal(str, i);

			if (result == special_literal::line_break)
				tkn += "\n";

			if (result != special_literal::none) {
				i++;
				continue;
			}
		}

		tkn += str[i];

		if (!string_literal && str[i] == '\n') {
			result.push_back(new Token(tkn));
			tkn = "";
			continue;
		}
	}

	if (tkn != "" && !string_literal)
		result.push_back(new Token(tkn));

	/*
	for (int i = 0; i < result.size(); i++) {
		std::cout << result[i]->identifier << std::endl;
	}
	*/

	return result;
}