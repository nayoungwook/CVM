#include "frame.h"

bool is_digit(std::string str) {
	for (int i = 0; i < str.size(); i++) {
		if (!(isdigit(str[i]) || str[i] == '.' || str[i] == 'f'))
			return false;
	}

	return true;
}

Memory* operand_to_memory(const std::string& operand, Machine* machine, FunctionFrame* frame, Command* command) {
	bool _is_digit = is_digit(operand) || ((operand[0] == '-' || operand[0] == '+') && is_digit(operand.substr(1, operand.size() - 1)));
	bool _is_identifier = !_is_digit;
	bool _is_string_literal = _is_identifier && operand[0] == '\"';
	bool _is_character = _is_identifier && operand[0] == '\'' && operand[operand.size() - 1] == '\'';

	Memory* result = new Memory(_unknown, "unknown");

	if (_is_digit) {

		if (operand[operand.size() - 1] == 'f') {
			float value = std::stof(operand);

			result->data = new FloatData(value);
		}
		else {
			long double value = std::stod(operand);
			if (floor(value) == value) {
				result->data = new IntegerData((int)value);
			}
			else {
				result->data = new NumberData(value);
			}
		}

		result->type = result->data->type;
		return result;
	}
	else if (_is_identifier) {
		if (_is_string_literal) {
			result->data = new StringLiteralData(operand);

			result->type = result->data->type;
			return result;
		}
		else if (_is_character) {
			result->data = new CharacterData(operand[1]);

			result->type = result->data->type;
			return result;
		}
		else {
			if (operand == "this") {
				if (frame->caller_class == nullptr) {
					new FailedToFindThisError(command->line_number);
				}
				return frame->caller_class;
			}
			else if (operand == "super") {
				if (dynamic_cast<ClassData*>(frame->caller_class->data)->parent_memory == nullptr) {
					new FailedToFindSuperError(command->line_number);
				}
				return dynamic_cast<ClassData*>(frame->caller_class->data)->parent_memory;
			}
			else if (operand == "null") {
				result->data = new NullData();
				result->type = result->data->type;
				return result;
			}
			else if (operand == "true" || operand == "false") {
				result->data = new BoolData(operand == "true");
				result->type = result->data->type;
				return result;
			}
			else {
				return load_memory(machine, frame, operand, command);
			}
		}
	}

	return nullptr;
}

Memory* load_memory(Machine* machine, FunctionFrame* frame, const std::string& name, Command* command) {
	bool _scope_exists = frame != nullptr && !frame->scopes.empty();
	bool _exist_in_locals = frame != nullptr && frame->local_memories.find(name) != frame->local_memories.end();
	bool _exist_in_class = false;
	bool _exist_in_global = machine->global_memories.find(name) != machine->global_memories.end();

	if (_scope_exists) {
		for (int i = 0; i < frame->scopes.size(); i++) {
			Scope* scope = frame->scopes[frame->scopes.size() - 1 - i];
			bool _exist_in_scope = scope->memories.find(name) != scope->memories.end();

			if (_exist_in_scope) {
				return scope->memories.find(name)->second;
			}
		}
	}

	Memory* class_found_memory = nullptr;

	if (frame != nullptr && frame->caller_class != nullptr) {
		ClassData* class_data = dynamic_cast<ClassData*>(frame->caller_class->data);
		class_found_memory = class_data->find_full_access_memory(name);
	}

	_exist_in_class = class_found_memory != nullptr;

	if (_exist_in_locals) {
		return frame->local_memories[name];
	}
	else if (_exist_in_class) {
		return class_found_memory;
	}
	else if (_exist_in_global) {
		return machine->global_memories[name];
	}
	else {
		new VariableNotFoundError(name, command->line_number);
	}

	return nullptr;
}

std::pair<Operand*, Operand*> get_lhs_rhs(Machine* machine, FunctionFrame* frame) {
	Operand* lhs = frame->stack->peek();
	frame->stack->pop();

	Operand* rhs = frame->stack->peek();
	frame->stack->pop();

	return std::make_pair(lhs, rhs);
}

inline void find_label(Machine* machine, const std::string& index, int& i) {
	int line = machine->label_table.find(index)->second;
	i = line;
}

long double get_number_data(Data* data) {
	if (data->root_type == _number) {
		if (data->type == _integer) {
			return dynamic_cast<IntegerData*>(data)->data_value;
		}
		else if (data->type == _float) {
			return dynamic_cast<FloatData*>(data)->data_value;
		}
		else if (data->type == _number) {
			return dynamic_cast<NumberData*>(data)->data_value;
		}
	}
	else {
		exit(5);
	}

	return 0;
}

Data* change_type_of_class(Machine* machine, Memory* memory, const std::string& name, Memory* target_memory) {
	std::string target_memory_name = get_parent_data(machine, machine->get_class_memory(dynamic_cast<ClassData*>(target_memory->data)->code_memory->name))->name;
	ClassCodeMemory* _change_code_memory = machine->get_class_memory(name);
	std::string _chage_name = get_parent_data(machine, _change_code_memory)->name;

	if (target_memory_name != _chage_name) {
		std::cout << "Error! class type of lhs does not match with rhs | ";
		exit(1);
	}

	return memory->data;
}

Data* change_data(Memory* memory, data_type type) {
	if (memory->data->root_type == _number) {
		if (type == _integer) {
			int data = (int)get_number_data(memory->data);
			return new IntegerData(data);
		}
		else if (type == _float) {
			float data = (float)get_number_data(memory->data);
			return new FloatData(data);
		}
		else if (type == _number) {
			long double data = (long double)get_number_data(memory->data);
			return new NumberData(data);
		}
	}

	return memory->data;
}

void declarate_memory(Machine* machine, std::map<std::string, Memory*>& memory_area, const std::string& name, FunctionFrame* frame, const std::string& memory_name, Command* command) {
	if (memory_area.find(name) == memory_area.end()) {
		data_type declare_type = _unknown;
		bool _is_class = machine->class_code_memories.find(memory_name) != machine->class_code_memories.end();
		bool _is_object = machine->object_code_memories.find(memory_name) != machine->object_code_memories.end();
		bool _is_scene = machine->scene_code_memories.find(memory_name) != machine->scene_code_memories.end();

		if (_is_class)
			declare_type = _class;
		else if (_is_scene)
			declare_type = _scene;
		else if (_is_object)
			declare_type = _object;
		else
			declare_type = data_type_from_string[memory_name];

		if (declare_type == _unknown) {
			new UnknownTypeError(name, command->line_number);
		}

		Memory* result = get_declaration_memory(name, frame);
		result->declaration_name = memory_name;
		Data* data = change_data(result, declare_type);

		if (!((_is_object || _is_class || (declare_type == _vector || declare_type == _texture)) && data->type == _null)) {
			if (data->root_type == _class && machine->class_code_memories.find(memory_name) != machine->class_code_memories.end()) {
				std::string rhs = get_parent_data(machine, dynamic_cast<ClassData*>(result->data)->code_memory)->name;

				std::string lhs = get_parent_data(machine, machine->get_class_memory(memory_name))->name;

				if (lhs != rhs) {
					new FailedToStoreVariable(lhs, rhs, command->line_number);
				}
			}
			else if (data->root_type == _object && machine->object_code_memories.find(memory_name) != machine->object_code_memories.end()) {
				std::string rhs = get_parent_data(machine, dynamic_cast<ObjectData*>(result->data)->code_memory)->name;

				std::string lhs = get_parent_data(machine, machine->get_object_memory(memory_name))->name;

				if (lhs != rhs) {
					new FailedToStoreVariable(lhs, rhs, command->line_number);
				}
			}
			else if (data->root_type == _scene && machine->scene_code_memories.find(memory_name) == machine->scene_code_memories.end()) {
				new FailedToStoreVariable("scene", memory_name, command->line_number);
			}
			else {
				if (data->root_type != root_types[declare_type]) {
					new FailedToStoreVariable(memory_name, string_from_data_type[declare_type], command->line_number);
				}
			}
		}

		result->type = declare_type;
		result->data = data;

		memory_area.insert(std::make_pair(name, result));
	}
	else {
		new VariableAlreadyExistError(name, command->line_number);
		exit(1);
	}
}

void create_scope(FunctionFrame* frame) {
	frame->scopes.push_back(new Scope());
}

void end_scope(FunctionFrame* frame) {
	Scope* scope = frame->scopes[frame->scopes.size() - 1];
	frame->scopes.erase(frame->scopes.begin() + (frame->scopes.size() - 1));
	delete scope;
}

void run_initialize_function(Machine* machine, Memory* memory, Command* command) {
	FunctionFrame* initialize_function = new FunctionFrame("initialize");

	if (memory->data->root_type == _class) {
		initialize_function->code_memory = dynamic_cast<ClassData*>(memory->data)->code_memory->initialize_function;
	}
	else if (memory->data->root_type == _object) {
		initialize_function->code_memory = dynamic_cast<ObjectData*>(memory->data)->code_memory->initialize_function;
	}
	else {
		initialize_function->code_memory = dynamic_cast<SceneData*>(memory->data)->code_memory->initialize_function;
	}

	initialize_function->caller_class = memory;

	initialize_function->run(machine, command);
}

BuiltInFunctionFrame* exchange_bulitin_function_frame(FunctionFrame* frame) {
	BuiltInFunctionFrame* result = new BuiltInFunctionFrame(frame->name);
	result->caller_class = frame->caller_class;
	result->caller_function = frame->caller_function;
	result->local_memories = frame->local_memories;

	return result;
}

void FunctionFrame::run(Machine* machine, Command* command) {
	std::vector<Command*> commands = this->code_memory->commands;
	for (int i = 0; i < commands.size(); i++) {
		Command* command = commands[i];

		bool _is_member_store = command->type == command_nmstore || command->type == command_pbstore || command->type == command_prstore || command->type == command_ptstore;
		bool _is_operator = command->type == command_add || command->type == command_sub || command->type == command_mult ||
			command->type == command_div || command->type == command_mod || command->type == command_pow;
		bool _is_compare_operator = command->type == command_equal ||
			command->type == command_not_equal ||
			command->type == command_lesser ||
			command->type == command_greater ||
			command->type == command_eq_lesser ||
			command->type == command_eq_greater;
		bool _is_logic_operator = command->type == command_or || command->type == command_and;

		if (command->type == command_push) {
			run_command_push(command, machine, this);
		}
		else if (command->type == command_array_get) {
			run_command_array_get(command, machine, this);
		}
		else if (command->type == command_store) {
			run_command_store(command, machine, this);
		}
		else if (_is_member_store) {
			run_command_member_store(command, machine, this);
		}
		else if (command->type == command_not) {
			run_command_not(command, machine, this);
		}
		else if (_is_operator) {
			run_command_operator(command, machine, this);
		}
		else if (_is_compare_operator) {
			run_command_compare_operator(command, machine, this);
		}
		else if (_is_logic_operator) {
			run_command_logic_operator(command, machine, this);
		}
		else if (command->type == command_call) {
			run_command_call(command, machine, this);
		}
		else if (command->type == command_create) {
			run_command_create(command, machine, this);
		}
		else if (command->type == command_return) {
			if (this->caller_function != nullptr) {
				Operand* return_value = this->stack->peek();
				Memory* return_memory = operand_to_memory(return_value->get_data(), machine, this, command);
				Data* return_data = change_data(return_memory, data_type_from_string[this->code_memory->return_type]);

				this->caller_function->stack->push(return_value);
				this->stack->pop();
			}

			return;
		}
		else if (command->type == command_load_identifier) {
			run_command_load_identifier(command, machine, this);
		}
		else if (command->type == command_load_call) {
			run_command_load_call(command, machine, this);
		}
		else if (command->type == command_label) {
			continue;
		}
		else if (command->type == command_if) {
			Operand* peek_data = this->stack->peek();
			stack->pop();
			if (peek_data->get_type() == operand_bool) {
				if (peek_data->get_data() == "false") {
					find_label(machine, command->operands[0]->identifier, i);
					continue;
				}
			}
		}
		else if (command->type == command_goto) {
			find_label(machine, command->operands[0]->identifier, i);
			continue;
		}
		else if (command->type == command_create_scope) {
			create_scope(this);
		}
		else if (command->type == command_end_scope) {
			end_scope(this);
		}
		else if (command->type == command_for) {
			Operand* peek_data = this->stack->peek();
			stack->pop();
			if (peek_data->get_type() == operand_bool) {
				if (peek_data->get_data() == "true") {
					find_label(machine, command->operands[0]->identifier, i);
					continue;
				}
			}
		}
		else if (command->type == command_array) {
			/*
			Memory* result_memory = new Memory(_array, "array");
			std::vector<Memory*> elements;

			std::string type = "";

			for (int i = 0; i < std::stoi(command->operands[0]->identifier); i++) {
				Memory* element = stack->peek();
				elements.push_back(element);
				stack->pop();
			}

			std::reverse(elements.begin(), elements.end());

			ArrayData* array_data = new ArrayData(elements);
			result_memory->data = array_data;

			stack->push(result_memory);
			*/
		}
		else if (command->type == command_vector) {
			/*
			Memory* result_memory = new Memory(_vector, "vector");

			std::vector<Memory*> elements;

			std::string type = "";
			size_t size = std::stoi(command->operands[0]->identifier);

			for (int i = 0; i < size; i++) {
				Memory* element = stack->peek();

				element->type = _float;
				element->declaration_name = "float";

				FloatData* element_data = new FloatData((float)get_number_data(element->data));
				free(element->data);
				element->data = element_data;

				elements.push_back(element);
				stack->pop();

				if (element->data->root_type != _number) {
					std::cout << "Error! element of vector must be number type." << std::endl;
					exit(1);
				}
			}

			if (size == 4) // reverse x, y, z to make t first index.
				std::reverse(elements.begin() + 1, elements.end());
			else
				std::reverse(elements.begin(), elements.end());

			VectorData* vector_data = new VectorData(elements);
			result_memory->data = vector_data;

			stack->push(result_memory);
			*/
		}
		else if (command->type == command_incre || command->type == command_decre) {
			Operand* peek_memory = stack->peek();
			stack->pop();

			if (peek_memory->get_type() == operand_number) {
				if (command->type == command_incre)
					stack->push(new Operand(std::to_string(std::stold(peek_memory->get_data()) + 1), operand_number));
				if (command->type == command_decre)
					stack->push(new Operand(std::to_string(std::stold(peek_memory->get_data()) - 1), operand_number));
			}
			else {
				std::cout << "Error! incre data is not number" << std::endl;
				exit(1);
			}
		}
	}

	if (this->code_memory->type != initialize_function_code_memory && this->code_memory->type != constructor_function_code_memory) {
		if (this->code_memory->return_type != "void") {
			new FunctionMustReturnValueError(this->name, this->code_memory->return_type, command->line_number);
		}
	}

	/*
	if (this->code_memory->return_type == "void") {
		if (this->caller_function != nullptr) {
			Memory* return_value = new Memory(_null, "null");
			return_value->data = new NullData();

			this->caller_function->stack->push(return_value);
		}
	}
	*/

	machine->function_frames.erase(remove(machine->function_frames.begin(), machine->function_frames.end(), this), machine->function_frames.end());
	delete this;
}

std::string get_memory_data_print_value(Memory* mem, Machine* machine, FunctionFrame* caller) {
	if (mem->data->type == _number) {
		long double value = dynamic_cast<NumberData*>(mem->data)->data_value;
		if ((int)value == value)
			return std::to_string((int)value);
		else
			return std::to_string(value);
	}
	else if (mem->data->type == _integer) {
		return std::to_string(dynamic_cast<IntegerData*>(mem->data)->data_value);
	}
	else if (mem->data->type == _float) {
		float value = dynamic_cast<FloatData*>(mem->data)->data_value;
		if ((int)value == value)
			return std::to_string((int)value);
		else
			return std::to_string(value);
	}
	else if (mem->data->root_type == _string) {
		return dynamic_cast<StringLiteralData*>(mem->data)->extracted_value;
	}
	else if (mem->data->root_type == _bool) {
		return dynamic_cast<BoolData*>(mem->data)->data_value ? "true" : "false";
	}
	else if (mem->data->root_type == _character) {
		return dynamic_cast<CharacterData*>(mem->data)->data_value + "";
	}
	else if (mem->data->root_type == _class) {
		ClassData* data = ((ClassData*)mem->data);
		return "<class | type : " + data->code_memory->name + " >";
	}
	else if (mem->data->root_type == _object) {
		ClassData* data = ((ClassData*)mem->data);
		return "<object | type : " + data->code_memory->name + " >";
	}
	else if (mem->data->root_type == _scene) {
		SceneData* data = ((SceneData*)mem->data);
		return "<scene | type : " + data->code_memory->name + " >";
	}
	else if (mem->data->root_type == _null) {
		return "null";
	}
	else if (mem->data->root_type == _array) {
		ArrayData* data = dynamic_cast<ArrayData*>(mem->data);
		std::string result = "";

		result += "{ ";
		for (int i = 0; i < data->data_value.size(); i++) {
			result += get_memory_data_print_value(data->data_value[i], machine, caller);

			if (i != data->data_value.size() - 1)
				result += " , ";
		}
		result += " }";

		return result;
	}
	else if (mem->data->root_type == _vector) {
		VectorData* data = dynamic_cast<VectorData*>(mem->data);
		std::string result = "";

		result += "( ";
		for (int i = 0; i < data->data_value.size(); i++) {
			result += get_memory_data_print_value(data->data_value[i], machine, caller);

			if (i != data->data_value.size() - 1)
				result += " , ";
		}
		result += " )";

		return result;
	}

	return "Unknown";
}

void BuiltInFunctionFrame::run(Machine* machine, FunctionFrame* caller, Command* command) {
	((BUILTIN::BUILTIN_FUNCTION*)this->built_in_code_memory)->run_builtin(machine, this, caller, command);
}