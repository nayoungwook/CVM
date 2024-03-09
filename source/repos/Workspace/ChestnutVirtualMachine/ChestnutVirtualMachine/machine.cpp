#include "machine.h"

Machine::Machine() {
}

void Machine::add_code_to_memory(const std::string& name, std::vector<Token*> tokens) {
	if (this->memory_code.find(name) != this->memory_code.end()) {
		this->memory_code.clear();
	}

	this->memory_code.insert(std::make_pair(name, tokens));
}

Token* Machine::get_token(std::vector<Token*>& tokens) {
	if (tokens.size() == 0) {
		std::cout << "Error! no tokens to read." << std::endl;
		exit(0);
	}

	return tokens[0];
}

void Machine::remove_token(std::vector<Token*>& tokens) {
	if (tokens.size() == 0) return;

	tokens.erase(tokens.begin());
}

std::vector<Token*> Machine::get_body_tokens(std::vector<Token*>& tokens) {
	if (get_token(tokens)->identifier != "{") {
		std::cout << "Error! body tokens does not begin with \"{\"" << std::endl;
		return {};
	}
	remove_token(tokens);

	int bracket_count = 1;

	int index = 0;

	std::vector<Token*> body_tokens;

	while (true) {
		if (get_token(tokens)->identifier == "{") {
			bracket_count++;
		}
		if (get_token(tokens)->identifier == "}") {
			bracket_count--;

			if (bracket_count == 0) {
				remove_token(tokens);
				break;
			}
		}

		body_tokens.push_back(get_token(tokens));
		remove_token(tokens);
	}

	return body_tokens;
}

std::vector<Token*>& Machine::load_code_memory(const std::string& name) {
	if (this->memory_code.find(name) == this->memory_code.end()) {
		std::cout << "Error : can\'t find code data from machine\'s memory" << std::endl;
		exit(0);
	}

	return this->memory_code[name];
}

ClassCodeMemory* Machine::get_class_memory(const std::string& name) {
	if (class_code_memories.find(name) == class_code_memories.end()) {
		std::cout << "Error! can\'t find class name : " << name << std::endl;
		exit(1);
	}

	return class_code_memories[name];
}

ObjectCodeMemory* Machine::get_object_memory(const std::string& name) {
	if (object_code_memories.find(name) == object_code_memories.end()) {
		std::cout << "Error! can\'t find object name : " << name << std::endl;
		exit(1);
	}

	return object_code_memories[name];
}

SceneCodeMemory* Machine::get_scene_memory(const std::string& name) {
	if (scene_code_memories.find(name) == scene_code_memories.end()) {
		std::cout << "Error! can\'t find class name : " << name << std::endl;
		exit(1);
	}

	return scene_code_memories[name];
}

FunctionCodeMemory* Machine::get_member_function_memory(ClassCodeMemory* frame, std::string const& name, std::vector<Memory*>& parameter_memories) {
	if (frame->member_functions.find(name) == frame->member_functions.end()) {
		std::cout << "Error! can\'t find function name : " << name << std::endl;
		exit(1);
	}

	FunctionBundle* found_function_bundle = frame->member_functions.find(name)->second;

	std::string type_string = types_to_string_format(this, parameter_memories);
	if (found_function_bundle->code_memories.find(type_string) != found_function_bundle->code_memories.end()) {
		return found_function_bundle->code_memories.find(type_string)->second;
	}
	else {
		std::cout << "Error! can\'t find function name with parameters : " << name << std::endl;
		exit(1);
	}

	return nullptr;
}

std::vector<Command*> Machine::get_commands(std::vector<Token*> body_tokens, FunctionCodeMemory* function_code_memory) {
	std::vector<Command*> result;
	int line_number = 0;

	while (!body_tokens.empty()) {
		Command* command = new Command();

		std::string command_str = "";

		command_str = get_token(body_tokens)->identifier.substr(1, get_token(body_tokens)->identifier.size() - 1);
		remove_token(body_tokens);

		std::vector<Token*> operands;
		int operands_count = 0;

		if (command_str == "push") {
			command->type = command_push;
			operands_count = 2;
		}
		else if (command_str == "store") {
			command->type = command_store;
		}
		else if (command_str == "nmstore") {
			command->type = command_nmstore;
			operands_count = 2;
		}
		else if (command_str == "prstore") {
			command->type = command_prstore;
			operands_count = 2;
		}
		else if (command_str == "pbstore") {
			command->type = command_pbstore;
			operands_count = 2;
		}
		else if (command_str == "ptstore") {
			command->type = command_ptstore;
			operands_count = 2;
		}
		else if (command_str == "call") {
			command->type = command_call;
			operands_count = 2;
		}
		else if (command_str == "add") {
			command->type = command_add;
		}
		else if (command_str == "sub") {
			command->type = command_sub;
		}
		else if (command_str == "div") {
			command->type = command_div;
		}
		else if (command_str == "mult") {
			command->type = command_mult;
		}
		else if (command_str == "pow") {
			command->type = command_pow;
		}
		else if (command_str == "mod") {
			command->type = command_mod;
		}
		else if (command_str == "create") {
			command->type = command_create;
			operands_count = 2;
		}
		else if (command_str == "ret") {
			command->type = command_return;
		}
		else if (command_str == "load_call") {
			command->type = command_load_call;
			operands_count = 2;
		}
		else if (command_str == "load_identifier") {
			command->type = command_load_identifier;
			operands_count = 1;
		}
		else if (command_str == "equal") {
			command->type = command_equal;
		}
		else if (command_str == "not_equal") {
			command->type = command_not_equal;
		}
		else if (command_str == "lesser") {
			command->type = command_lesser;
		}
		else if (command_str == "greater") {
			command->type = command_greater;
		}
		else if (command_str == "equal_lesser") {
			command->type = command_eq_lesser;
		}
		else if (command_str == "equal_greater") {
			command->type = command_eq_greater;
		}
		else if (command_str == "and") {
			command->type = command_and;
		}
		else if (command_str == "or") {
			command->type = command_or;
		}
		else if (command_str == "not") {
			command->type = command_not;
		}
		else if (command_str == "goto") {
			command->type = command_goto;
			operands_count = 1;
		}
		else if (command_str == "if") {
			command->type = command_if;
			operands_count = 1;
		}
		else if (command_str == "for") {
			command->type = command_for;
			operands_count = 1;
		}
		else if (command_str == "array") {
			command->type = command_array;
			operands_count = 1;
		}
		else if (command_str == "vector") {
			command->type = command_vector;
			operands_count = 1;
		}
		else if (command_str == "label") {
			command->type = command_label;
			operands_count = 1;

			this->label_table.insert(std::make_pair(get_token(body_tokens)->identifier, line_number));
		}
		else if (command_str == "create_scope") {
			command->type = command_create_scope;
		}
		else if (command_str == "end_scope") {
			command->type = command_end_scope;
		}
		else if (command_str == "array_get") {
			command->type = command_array_get;
		}
		else if (command_str == "incre") {
			command->type = command_incre;
		}
		else if (command_str == "decre") {
			command->type = command_decre;
		}
		else {
			std::cout << "Error! unknown command : " << command_str << std::endl;
			exit(1);
		}

		for (int i = 0; i < operands_count; i++) {
			operands.push_back(get_token(body_tokens));
			remove_token(body_tokens);
		}

		command->line_number = std::stoi(get_token(body_tokens)->identifier);
		remove_token(body_tokens);

		command->operands = operands;
		result.push_back(command);
		line_number++;
	}

	return result;
}

std::string to_upper_all(const std::string& str) {
	std::string result;

	for (int i = 0; i < str.size(); i++) {
		result += std::toupper(str[i]);
	}

	return result;
}

void store_function(Machine* machine, FunctionCodeMemory* func_memory, ClassCodeMemory* class_memory) {
	std::string type_string = types_to_string_format(machine, func_memory);

	bool same_name_of_function_exist = class_memory->member_functions.find(func_memory->name) != class_memory->member_functions.end();

	if (!same_name_of_function_exist) {
		FunctionBundle* bundle = new FunctionBundle(func_memory->access_modifier);
		bundle->code_memories.insert(std::make_pair(type_string, func_memory));
		class_memory->member_functions.insert(std::make_pair(func_memory->name, bundle));
	}
	else {
		bool same_name_and_type_of_function_exist = class_memory->member_functions.find(func_memory->name)->second->code_memories.find(type_string)
			!= class_memory->member_functions.find(func_memory->name)->second->code_memories.end();

		if (!same_name_and_type_of_function_exist) {
			class_memory->member_functions.find(func_memory->name)->second->code_memories.insert(std::make_pair(type_string, func_memory));
		}
		else {
			std::cout << "function of same name and type exist." << std::endl;
			exit(1);
		}
	}
}

CodeMemory* Machine::memory_tokens_to_code_memory(std::vector<Token*>& memory_tokens) {
	if (memory_tokens.size() == 0) return nullptr;

	Token* first_token = get_token(memory_tokens);
	remove_token(memory_tokens);

	if (first_token->identifier[0] == '.' || first_token->identifier[0] == '$') { // function
		std::string name = first_token->identifier.substr(1, first_token->identifier.size() - 1);

		std::string access_modifier = get_token(memory_tokens)->identifier;
		remove_token(memory_tokens);

		bool is_static = get_token(memory_tokens)->identifier == "1";
		remove_token(memory_tokens);

		std::string return_type = get_token(memory_tokens)->identifier;
		remove_token(memory_tokens);

		FunctionCodeMemory* function_memory = nullptr;

		if (first_token->identifier[0] == '$') {
			std::string func_name = first_token->identifier.substr(1, first_token->identifier.size() - 1);
			if (func_name == "initialize")
				function_memory = new InitializeFunctionCodeMemory(name, return_type, access_modifier);
			else if (func_name == "constructor")
				function_memory = new ConstructorFunctionCodeMemory(name, return_type, access_modifier);
		}
		else {
			function_memory = new FunctionCodeMemory(name, return_type, access_modifier);
		}

		if (function_memory != nullptr) {
			while (memory_tokens[0]->identifier != "{") {
				std::string name = get_token(memory_tokens)->identifier;
				remove_token(memory_tokens);
				std::string identifier = get_token(memory_tokens)->identifier;
				remove_token(memory_tokens);

				function_memory->parameters.push_back(std::make_pair(name, identifier));
			}

			std::vector<Token*> body_tokens = get_body_tokens(memory_tokens);
			std::vector<Command*> commands = get_commands(body_tokens, function_memory);

			function_memory->commands = commands;
			function_memory->name = name;
			function_memory->is_static = is_static;
		}

		return function_memory;
	}
	else if (first_token->identifier == "class") {
		std::string name = get_token(memory_tokens)->identifier;
		remove_token(memory_tokens);

		std::string parent_name = "";

		if (get_token(memory_tokens)->identifier != "{") {
			parent_name = get_token(memory_tokens)->identifier;
			remove_token(memory_tokens);
		}

		ClassCodeMemory* class_memory = new ClassCodeMemory(name);

		if (parent_name != "") {
			class_memory->parent_name = parent_name;
		}

		std::vector<Token*> body_tokens = get_body_tokens(memory_tokens);

		while (!body_tokens.empty()) {
			FunctionCodeMemory* func_memory = dynamic_cast<FunctionCodeMemory*>(memory_tokens_to_code_memory(body_tokens));

			if (func_memory->type == initialize_function_code_memory) {
				class_memory->initialize_function = dynamic_cast<InitializeFunctionCodeMemory*>(func_memory);
			}
			else if (func_memory->type == constructor_function_code_memory) {
				std::string type_string = types_to_string_format(this, func_memory);
				if (class_memory->constructor_function == nullptr)
					class_memory->constructor_function = new FunctionBundle(func_memory->access_modifier);

				class_memory->constructor_function->code_memories.insert(std::make_pair(type_string, dynamic_cast<ConstructorFunctionCodeMemory*>(func_memory)));
			}
			else {
				store_function(this, func_memory, class_memory);
			}
		}

		return class_memory;
	}
	else if (first_token->identifier == "object") {
		std::string name = get_token(memory_tokens)->identifier;
		remove_token(memory_tokens);

		std::string parent_name = "";

		if (get_token(memory_tokens)->identifier != "{") {
			parent_name = get_token(memory_tokens)->identifier;
			remove_token(memory_tokens);
		}

		ObjectCodeMemory* object_memory = new ObjectCodeMemory(name);

		if (parent_name != "") {
			object_memory->parent_name = parent_name;
		}

		std::vector<Token*> body_tokens = get_body_tokens(memory_tokens);

		while (!body_tokens.empty()) {
			FunctionCodeMemory* func_memory = dynamic_cast<FunctionCodeMemory*>(memory_tokens_to_code_memory(body_tokens));

			if (func_memory->type == initialize_function_code_memory) {
				object_memory->initialize_function = dynamic_cast<InitializeFunctionCodeMemory*>(func_memory);
			}
			else if (func_memory->type == constructor_function_code_memory) {
				std::string type_string = types_to_string_format(this, func_memory);
				if (object_memory->constructor_function == nullptr)
					object_memory->constructor_function = new FunctionBundle(func_memory->access_modifier);

				object_memory->constructor_function->code_memories.insert(std::make_pair(type_string, dynamic_cast<ConstructorFunctionCodeMemory*>(func_memory)));
			}
			else {
				store_function(this, func_memory, object_memory);
			}
		}

		return object_memory;
	}
	else if (first_token->identifier == "scene") {
		std::string name = get_token(memory_tokens)->identifier;
		remove_token(memory_tokens);

		SceneCodeMemory* scene_memory = new SceneCodeMemory(name);

		std::vector<Token*> body_tokens = get_body_tokens(memory_tokens);

		while (!body_tokens.empty()) {
			FunctionCodeMemory* func_memory = dynamic_cast<FunctionCodeMemory*>(memory_tokens_to_code_memory(body_tokens));

			if (func_memory->type == initialize_function_code_memory) {
				scene_memory->initialize_function = dynamic_cast<InitializeFunctionCodeMemory*>(func_memory);
			}
			else if (func_memory->type == constructor_function_code_memory) {
				std::string type_string = types_to_string_format(this, func_memory);
				if (scene_memory->constructor_function == nullptr)
					scene_memory->constructor_function = new FunctionBundle(func_memory->access_modifier);
				scene_memory->constructor_function->code_memories.insert(std::make_pair(
					type_string, dynamic_cast<ConstructorFunctionCodeMemory*>(func_memory)
				));
			}
			else {
				store_function(this, func_memory, scene_memory);
			}
		}

		return scene_memory;
	}
	else if (first_token->identifier == "#import") {
		std::string name = get_token(memory_tokens)->identifier;
		name = name.substr(1, name.size() - 2);
		remove_token(memory_tokens);

		if (to_upper_all(name) == "BASIC") {
			Memory* current_scene_memory = new Memory(_scene, "scene");
			NullData* null_data = new NullData();
			current_scene_memory->data = null_data;
			global_memories.insert(std::make_pair("current_scene", current_scene_memory));

			class_code_memories.insert(std::make_pair("Window", new BUILTIN::WINDOW()));

			built_in_function_code_memories.insert(std::make_pair("print", new FunctionBundle(std::make_pair("", new BUILTIN::PRINT()), "public")));
			built_in_function_code_memories.insert(std::make_pair("initialize_engine", new FunctionBundle(std::make_pair("", new BUILTIN::INITIALIZE_ENGINE()), "public")));
			built_in_function_code_memories.insert(std::make_pair("create_window", new FunctionBundle(std::make_pair("stringnumbernumber", new BUILTIN::CREATE_WINDOW()), "public")));
			built_in_function_code_memories.insert(std::make_pair("create_texture", new FunctionBundle(std::make_pair("string", new BUILTIN::CREATE_TEXTURE()), "public")));
			built_in_function_code_memories.insert(std::make_pair("use_shader", new FunctionBundle(std::make_pair("shader", new BUILTIN::USE_SHADER()), "public")));

			built_in_function_code_memories.insert(std::make_pair("run_engine", new FunctionBundle(std::make_pair("Window", new BUILTIN::RUN_ENGINE()), "public")));
		}
		else if (to_upper_all(name) == "OPENGL") {
		}
		else if (to_upper_all(name) == "MATH") {
			Memory* pi_memory = new Memory(_number, "number");
			NumberData* pi_data = new NumberData(M_PI);
			pi_memory->data = pi_data;
			global_memories.insert(std::make_pair("PI", pi_memory));

			Memory* e_memory = new Memory(_number, "number");
			NumberData* e_data = new NumberData(M_E);
			e_memory->data = e_data;
			global_memories.insert(std::make_pair("E", e_memory));

			built_in_function_code_memories.insert(std::make_pair("sin", new FunctionBundle(std::make_pair("number", new BUILTIN::SIN()), "public")));
			built_in_function_code_memories.insert(std::make_pair("cos", new FunctionBundle(std::make_pair("number", new BUILTIN::COS()), "public")));
			built_in_function_code_memories.insert(std::make_pair("tan", new FunctionBundle(std::make_pair("number", new BUILTIN::TAN()), "public")));
			built_in_function_code_memories.insert(std::make_pair("sqrt", new FunctionBundle(std::make_pair("number", new BUILTIN::SQRT()), "public")));
		}
	}

	return nullptr;
}
