#include "parser.h"

Operator* get_operator(std::unordered_map<std::string, unsigned int>* label_id, int line_number, std::vector<Token*>& tokens) {
	Token* token = pull_token(tokens);

	Operator* result = nullptr;
	std::vector<Token*> operands;
	operator_type type = op_none;

	if (token->identifier == "@PUSH_STRING") {
		type = op_push_string;

		operands.push_back(pull_token(tokens)); // content
	}
	else if (token->identifier == "@PUSH_NUMBER") {
		type = op_push_number;

		operands.push_back(pull_token(tokens)); // content
	}
	else if (token->identifier == "@PUSH_BOOL") {
		type = op_push_bool;

		operands.push_back(pull_token(tokens)); // content
	}
	else if (token->identifier == "@STORE_GLOBAL") {
		type = op_store_global;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
	}
	else if (token->identifier == "@STORE_CLASS") {
		type = op_store_class;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
	}
	else if (token->identifier == "@STORE_ATTR") {
		type = op_store_attr;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
	}
	else if (token->identifier == "@STORE_LOCAL") {
		type = op_store_local;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
	}
	else if (token->identifier == "@LOAD_GLOBAL") {
		type = op_load_global;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
	}
	else if (token->identifier == "@LOAD_LOCAL") {
		type = op_load_local;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
	}
	else if (token->identifier == "@ADD") {
		type = op_add;
	}
	else if (token->identifier == "@SUB") {
		type = op_sub;
	}
	else if (token->identifier == "@MUL") {
		type = op_mul;
	}
	else if (token->identifier == "@DIV") {
		type = op_div;
	}
	else if (token->identifier == "@POW") {
		type = op_pow;
	}
	else if (token->identifier == "@MOD") {
		type = op_mod;
	}
	else if (token->identifier == "@RET") {
		type = op_ret;
	}
	else if (token->identifier == "@OR") {
		type = op_or;
	}
	else if (token->identifier == "@AND") {
		type = op_and;
	}
	else if (token->identifier == "@GREATER") {
		type = op_greater;
	}
	else if (token->identifier == "@LESSER") {
		type = op_lesser;
	}
	else if (token->identifier == "@EQ_GREATER") {
		type = op_eq_greater;
	}
	else if (token->identifier == "@EQ_LESSER") {
		type = op_eq_lesser;
	}
	else if (token->identifier == "@EQUAL") {
		type = op_equal;
	}
	else if (token->identifier == "@NOT_EQUAL") {
		type = op_not_equal;
	}
	else if (token->identifier == "@INCRE") {
		type = op_inc;
	}
	else if (token->identifier == "@DECRE") {
		type = op_dec;
	}
	else if (token->identifier == "@LABEL") {
		type = op_label;

		Token* tkn = pull_token(tokens);
		std::string id = tkn->identifier;
		label_id->insert(std::make_pair(id, line_number));

		operands.push_back(tkn); // label
	}
	else if (token->identifier == "@GOTO") {
		type = op_goto;

		operands.push_back(pull_token(tokens)); // label
	}
	else if (token->identifier == "@FOR") {
		type = op_for;

		operands.push_back(pull_token(tokens)); // label
	}
	else if (token->identifier == "@IF") {
		type = op_if;

		operands.push_back(pull_token(tokens)); // label
	}
	else if (token->identifier == "@LOAD_ATTR") {
		type = op_load_attr;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
	}
	else if (token->identifier == "@NEW") {
		type = op_new;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens); // ( name )

		operands.push_back(pull_token(tokens)); // constructor parameter count
	}
	else if (token->identifier == "@CALL_BUILTIN" || token->identifier == "@CALL_GLOBAL" || token->identifier == "@CALL_ATTR") {
		if (token->identifier == "@CALL_BUILTIN")
			type = op_call_builtin;
		else if (token->identifier == "@CALL_GLOBAL")
			type = op_call_global;
		else if (token->identifier == "@CALL_ATTR")
			type = op_call_attr;

		operands.push_back(pull_token(tokens)); // id
		pull_token(tokens);
		operands.push_back(pull_token(tokens)); // parameter_count
	}

	std::string line_number_str = pull_token(tokens)->identifier;

	int code_line_number = std::stoi(line_number_str);
	result = new Operator(type, operands, code_line_number);

	assert(result != nullptr);

	return result;
}

CodeMemory* get_code_memory(std::unordered_map<std::string, unsigned int>* label_id, std::vector<Token*>& tokens) {
	Token* token = pull_token(tokens);

	if (token->identifier == "FUNC" || token->identifier == "$INITIALIZE" || token->identifier == "$CONSTRUCTOR") {
		unsigned int id = std::stoi(pull_token(tokens)->identifier);
		std::string name = pull_token(tokens)->identifier; // ( name )
		std::string access_modifier = pull_token(tokens)->identifier;

		std::vector<std::string> params;

		while (tokens[0]->identifier != "{") {
			params.push_back(pull_token(tokens)->identifier);
		}

		std::string return_type = params[params.size() - 1];

		params.erase(params.begin() + params.size() - 1);

		pull_token(tokens); // {

		std::vector<Operator*> operators;

		int i = 0;
		while (tokens[0]->identifier != "}") {
			operators.push_back(get_operator(label_id, i, tokens));
			i++;
		}

		pull_token(tokens); // }

		CodeMemory* result = nullptr;

		if (token->identifier == "$INITIALIZE")
			result = (CodeMemory*) new CMInitialize(operators, id, return_type, params, access_modifier);
		else  if (token->identifier == "$CONSTRUCTOR")
			result = (CodeMemory*) new CMConstructor(operators, id, return_type, params, access_modifier);
		else
			result = (CodeMemory*) new CMFunction(operators, id, return_type, params, access_modifier);

		((CMFunction*)result)->name = name.substr(1, name.size() - 2);

		return result;
	}
	else if (token->identifier == "CLASS" || token->identifier == "SCENE" || token->identifier == "OBJECT") {
		std::string object_type = token->identifier;
		unsigned int id = std::stoi(pull_token(tokens)->identifier);
		std::string name = pull_token(tokens)->identifier; // ( name )
		unsigned int parent_id = std::stoi(pull_token(tokens)->identifier);

		pull_token(tokens); // {

		std::unordered_map<unsigned int, CMFunction*>* public_function = new std::unordered_map<unsigned int, CMFunction*>;
		std::unordered_map<unsigned int, CMFunction*>* private_function = new std::unordered_map<unsigned int, CMFunction*>;
		std::unordered_map<unsigned int, CMFunction*>* protected_function = new std::unordered_map<unsigned int, CMFunction*>;
		std::unordered_map<unsigned int, CMFunction*>* default_function = new std::unordered_map<unsigned int, CMFunction*>;

		CMInitialize* initializer = nullptr;
		CMConstructor* constructor = nullptr;

		unsigned int
			init_function_id = -1,
			tick_function_id = -1,
			render_function_id = -1;

		while (tokens[0]->identifier != "}") {
			CMFunction* member_function = (CMFunction*)get_code_memory(label_id, tokens);

			if (member_function->get_type() == code_constructor) {
				constructor = (CMConstructor*)member_function;
			}
			else if (member_function->get_type() == code_initialize) {
				initializer = (CMInitialize*)member_function;
			}
			else {
				if (member_function->get_access_modifier() == "public") {
					unsigned int id = (unsigned int)public_function->size();
					std::string function_name = member_function->name;

					if (function_name == "init")
						init_function_id = id;
					else if (function_name == "tick")
						tick_function_id = id;
					else if (function_name == "render")
						render_function_id = id;

					public_function->insert(std::make_pair(id, member_function));
				}
				else if (member_function->get_access_modifier() == "private")
					private_function->insert(std::make_pair((unsigned int)private_function->size(), member_function));
				else if (member_function->get_access_modifier() == "protected")
					protected_function->insert(std::make_pair((unsigned int)protected_function->size(), member_function));
				else if (member_function->get_access_modifier() == "default")
					default_function->insert(std::make_pair((unsigned int)default_function->size(), member_function));
			}
		}

		pull_token(tokens); // }

		CodeMemory* result = nullptr;

		if (object_type == "CLASS")
			result = (CodeMemory*) new CMClass(id, parent_id,
				init_function_id, tick_function_id, render_function_id);
		else if (object_type == "SCENE")
			result = (CodeMemory*) new CMScene(id, parent_id,
				init_function_id, tick_function_id, render_function_id);

		((CMClass*)result)->public_function = public_function;
		((CMClass*)result)->private_function = private_function;
		((CMClass*)result)->protected_function = protected_function;
		((CMClass*)result)->default_function = default_function;

		((CMClass*)result)->constructor = constructor;
		((CMClass*)result)->initializer = initializer;

		((CMClass*)result)->name = name.substr(1, name.size() - 2);

		return result;
	}

	return nullptr;
}

Token* pull_token(std::vector<Token*>& tokens) {
	Token* result = tokens.front();
	tokens.erase(tokens.begin());
	return result;
}

special_literal check_special_literal(const std::string& str, int i) {
	if (str.size() > i + 1) {
		if (str[i + 1] == 'n') {
			return special_literal::line_break;
		}
	}
	return special_literal::none;
}

std::vector<Token*> extract_tokens(std::string const& str) {

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