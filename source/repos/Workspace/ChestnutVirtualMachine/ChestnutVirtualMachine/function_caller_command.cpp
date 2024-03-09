#include "function_caller_command.h"

std::vector<Memory*> get_parameter_memories(int parameter_count, FunctionFrame* caller_frame) {
	std::vector<Memory*> parameters;

	for (int i = 0; i < parameter_count; i++) {
		Memory* memory = caller_frame->stack->peek();
		parameters.push_back(memory);
		caller_frame->stack->pop();
	}
	return parameters;
}

FunctionFrame* get_class_caller_function_data(Machine* machine, FunctionFrame* caller_frame, std::vector<Memory*> parameters, const std::string& name) {
	ClassCodeMemory* caller_class_code_memory = dynamic_cast<ClassData*>(caller_frame->caller_class->data)->code_memory;

	if (caller_class_code_memory == nullptr) {
		std::cout << "error!" << std::endl;
		exit(1);
	}

	FunctionBundle* bundle = dynamic_cast<ClassData*>(caller_frame->caller_class->data)->find_full_access_function(name);

	FunctionFrame* callee_function_frame = new FunctionFrame(name);
	std::string type_string = types_to_string_format(machine, parameters);

	if (bundle->code_memories.find(type_string) != bundle->code_memories.end()) {
		callee_function_frame->caller_class = caller_frame->caller_class;
		callee_function_frame->code_memory = bundle->code_memories.find(type_string)->second;

		if (caller_frame->caller_class != nullptr)
			callee_function_frame->caller_class = caller_frame->caller_class;
	}

	return callee_function_frame;
}

FunctionFrame* get_scene_caller_function_data(Machine* machine, FunctionFrame* caller_frame, std::vector<Memory*> parameters, const std::string& name) {
	SceneCodeMemory* caller_class_code_memory = dynamic_cast<SceneCodeMemory*>(dynamic_cast<SceneData*>(caller_frame->caller_class->data)->code_memory);

	if (caller_class_code_memory == nullptr) {
		std::cout << "error!" << std::endl;
		exit(1);
	}
	if (caller_class_code_memory->member_functions.find(name) == caller_class_code_memory->member_functions.end()) {
		std::cout << "error!" << std::endl;
		exit(1);
	}

	FunctionBundle* bundle = caller_class_code_memory->member_functions[name];

	FunctionFrame* callee_function_frame = new FunctionFrame(name);
	std::string type_string = types_to_string_format(machine, parameters);

	if (bundle->code_memories.find(type_string) != bundle->code_memories.end()) {
		callee_function_frame->caller_class = caller_frame->caller_class;
		callee_function_frame->code_memory = bundle->code_memories.find(type_string)->second;

		if (caller_frame->caller_class != nullptr)
			callee_function_frame->caller_class = caller_frame->caller_class;
	}
	return callee_function_frame;
}

FunctionFrame* get_global_function_data(Machine* machine, FunctionFrame* caller_frame, std::vector<Memory*> parameters, const std::string& name) {

	if (machine->function_code_memories.find(name) == machine->function_code_memories.end()) {
		std::cout << "error!" << std::endl;
		exit(1);
	}

	FunctionBundle* bundle = machine->function_code_memories[name];

	FunctionFrame* callee_function_frame = new FunctionFrame(name);
	std::string type_string = types_to_string_format(machine, parameters);

	if (bundle->code_memories.find(type_string) != bundle->code_memories.end()) {
		callee_function_frame->caller_function = caller_frame;
		callee_function_frame->code_memory = bundle->code_memories.find(type_string)->second;

		if (caller_frame->caller_class != nullptr)
			callee_function_frame->caller_class = caller_frame->caller_class;
	}
	return callee_function_frame;
}

FunctionFrame* get_built_function_data(Machine* machine, FunctionFrame* caller_frame, std::vector<Memory*> parameters, const std::string& name) {
	if (machine->built_in_function_code_memories.find(name) == machine->built_in_function_code_memories.end()) {
		std::cout << "error!" << std::endl;
		exit(1);
	}

	FunctionFrame* callee_function_frame = new FunctionFrame(name);
	FunctionBundle* bundle = machine->built_in_function_code_memories[name];
	std::string type_string = types_to_string_format(machine, parameters);

	if (bundle->code_memories.find(type_string) != bundle->code_memories.end()) {
		callee_function_frame->caller_function = caller_frame;
		callee_function_frame->code_memory = bundle->code_memories.find(type_string)->second;

		if (caller_frame->caller_class != nullptr)
			callee_function_frame->caller_class = caller_frame->caller_class;
	}

	return callee_function_frame;
}

FunctionFrame* get_constructor_function_data(Machine* machine, FunctionFrame* caller_frame, ClassData* parent_data, std::vector<Memory*> parameters) {
	FunctionFrame* callee_function_frame = new FunctionFrame("constructor");
	FunctionBundle* bundle = dynamic_cast<ClassCodeMemory*>(parent_data->code_memory)->constructor_function;
	std::string type_string = types_to_string_format(machine, parameters);

	if (bundle->code_memories.find(type_string) != bundle->code_memories.end()) {
		callee_function_frame->caller_function = caller_frame;
		callee_function_frame->code_memory = bundle->code_memories.find(type_string)->second;

		if (caller_frame->caller_class != nullptr)
			callee_function_frame->caller_class = dynamic_cast<ClassData*>(caller_frame->caller_class->data)->parent_memory;
	}

	return callee_function_frame;
}

FunctionFrame* get_function_data(Machine* machine, FunctionFrame* caller_frame, const std::string& name, std::vector<Memory*>& parameters, Command* command) {

	bool found_in_class_caller = false;
	if (caller_frame->caller_class != nullptr) {
		ClassCodeMemory* caller_code_memory = ((ClassData*)caller_frame->caller_class->data)->code_memory;
		if (caller_code_memory != nullptr) {
			found_in_class_caller = caller_frame->caller_class != nullptr && ((ClassData*)caller_frame->caller_class->data)->find_full_access_function(name) != nullptr;
		}
	}

	bool found_in_scene_caller = false;
	if (caller_frame->caller_class != nullptr) {
		SceneCodeMemory* caller_code_memory = (SceneCodeMemory*)((SceneData*)caller_frame->caller_class->data)->code_memory;
		if (caller_code_memory != nullptr) {
			found_in_scene_caller = caller_frame->caller_class != nullptr && ((ClassData*)caller_frame->caller_class->data)->find_full_access_function(name) != nullptr;
		}
	}

	bool found_in_global = machine->function_code_memories.find(name) != machine->function_code_memories.end();
	bool found_in_builtin = machine->built_in_function_code_memories.find(name) != machine->built_in_function_code_memories.end();

	if (name == "super") {
		ClassData* class_data = dynamic_cast<ClassData*>(caller_frame->caller_class->data);
		std::string parent_name = dynamic_cast<ClassCodeMemory*>(class_data->code_memory)->parent_name;

		bool parent_exist = parent_name != "";

		if (parent_exist) {
			dynamic_cast<ClassData*>(caller_frame->caller_class->data)->parent_memory = create_class(command, machine, caller_frame, parent_name, parameters.size());
			run_initialize_function(machine, dynamic_cast<ClassData*>(caller_frame->caller_class->data)->parent_memory, command);
			return get_constructor_function_data(machine, caller_frame,
				dynamic_cast<ClassData*>(dynamic_cast<ClassData*>(caller_frame->caller_class->data)->parent_memory->data), parameters);
		}
		else {
			std::cout << "Error! this class doesn\'t have super class" << std::endl;
			exit(1);
		}
	}
	else {
		if (found_in_class_caller) {
			return get_class_caller_function_data(machine, caller_frame, parameters, name);
		}
		else if (found_in_scene_caller) {
			return get_scene_caller_function_data(machine, caller_frame, parameters, name);
		}
		else if (found_in_global) {
			return get_global_function_data(machine, caller_frame, parameters, name);
		}
		else if (found_in_builtin) {
			return get_built_function_data(machine, caller_frame, parameters, name);
		}
		else {
			new FunctionNotFoundError(name, types_to_string_format(machine, parameters), command->line_number);
			return nullptr;
		}
	}
}

void run_function(Machine* machine, FunctionFrame* caller_frame, FunctionFrame* callee_function_frame, const std::string& name,
	std::vector<Memory*>& parameters, Command* command) {

	bool found_in_caller = false;
	if (caller_frame->caller_class != nullptr) {
		ClassCodeMemory* caller_code_memory = dynamic_cast<ClassData*>(caller_frame->caller_class->data)->code_memory;

		found_in_caller = (caller_frame->caller_class != nullptr && caller_code_memory != nullptr && dynamic_cast<ClassData*>(caller_frame->caller_class->data)->find_full_access_function(name) != nullptr);

		if (found_in_caller) {
			FunctionBundle* bundle = dynamic_cast<ClassData*>(caller_frame->caller_class->data)->find_full_access_function(name);
			std::string type_string = types_to_string_format(machine, parameters);

			found_in_caller = (bundle->code_memories.find(type_string) != bundle->code_memories.end());
		}
	}

	bool found_in_global = machine->function_code_memories.find(name) != machine->function_code_memories.end();
	bool found_in_builtin = machine->built_in_function_code_memories.find(name) != machine->built_in_function_code_memories.end();
	bool is_super_constructor = name == "super";

	std::string type_string = types_to_string_format(machine, parameters);

	if (is_super_constructor) {
		int parameter_index = 0;
		for (Memory* memory : parameters) {
			callee_function_frame->local_memories.insert(std::make_pair(callee_function_frame->
				code_memory->parameters[parameter_index].first, memory));
			parameter_index++;
		}

		callee_function_frame->run(machine, command);
		return;
	}
	else if (found_in_caller || found_in_global) {
		if (found_in_caller && caller_frame->caller_class != nullptr) {
			int parameter_index = 0;
			for (Memory* memory : parameters) {
				callee_function_frame->local_memories.insert(std::make_pair(callee_function_frame->code_memory->parameters[parameter_index].first, memory));
				parameter_index++;
			}

			callee_function_frame->run(machine, command);
			return;
		}
		else if (found_in_global) {
			int parameter_index = 0;
			for (Memory* memory : parameters) {
				callee_function_frame->local_memories.insert(std::make_pair(callee_function_frame->code_memory->parameters[parameter_index].first, memory));
				parameter_index++;
			}

			callee_function_frame->run(machine, command);
			return;
		}
	}
	else if (found_in_builtin) {
		callee_function_frame = new BuiltInFunctionFrame(name);
		FunctionBundle* bundle = machine->built_in_function_code_memories.find(name)->second;
		bool is_infinite_parameters = false;

		BUILTIN::BUILTIN_FUNCTION* function_code_memory = nullptr;

		if (bundle->code_memories.find("") != bundle->code_memories.end()) {
			if (((BUILTIN::BUILTIN_FUNCTION*)bundle->code_memories[""])->is_infinite_parameters()) {
				is_infinite_parameters = true;
				function_code_memory = ((BUILTIN::BUILTIN_FUNCTION*)bundle->code_memories[""]);
			}
		}

		if (!is_infinite_parameters) {
			if (bundle->code_memories.find(type_string) != bundle->code_memories.end()) {
				function_code_memory = ((BUILTIN::BUILTIN_FUNCTION*)bundle->code_memories[type_string]);
			}
		}

		if (function_code_memory != nullptr) {
			(dynamic_cast<BuiltInFunctionFrame*>(callee_function_frame))->built_in_code_memory = function_code_memory;
			(dynamic_cast<BuiltInFunctionFrame*>(callee_function_frame))->built_in_code_memory->type = built_in_function_code_memory;

			int parameter_index = 0;
			for (Memory* memory : parameters) {
				std::string parameter_name;

				if (is_infinite_parameters) {
					parameter_name = std::to_string(parameter_index);
				}
				else {
					parameter_name = function_code_memory->parameters[parameter_index].first;
				}

				callee_function_frame->local_memories.insert(std::make_pair(parameter_name, memory));

				parameter_index++;
			}

			(dynamic_cast<BuiltInFunctionFrame*>(callee_function_frame))->run(machine, caller_frame, command);
			return;
		}
	}

	new FunctionNotFoundError(name, types_to_string_format(machine, parameters), command->line_number);
}

void run_command_call(Command* command, Machine* machine, FunctionFrame* caller_frame) {
	std::string name = command->operands[0]->identifier;
	int parameter_count = std::stoi(command->operands[1]->identifier);

	std::vector<Memory*> parameters = get_parameter_memories(parameter_count, caller_frame);

	FunctionFrame* callee_function_frame = get_function_data(machine, caller_frame, name, parameters, command);
	run_function(machine, caller_frame, callee_function_frame, name, parameters, command);
}