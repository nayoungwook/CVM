#include "object_create_command.h"

std::vector<Memory*> get_constructor_parameters(Command* command, FunctionFrame* frame) {

	int parameter_count = std::stoi(command->operands[1]->identifier);

	std::vector<Memory*> constructor_parameters;

	for (int i = 0; i < parameter_count; i++) {
		constructor_parameters.push_back(frame->stack->peek());
		frame->stack->pop();
	}

	return constructor_parameters;
}

void run_constructor_function(Machine* machine, Memory* result_memory, ClassData* class_data, std::vector<Memory*> constructor_parameters, Command* command) {

	std::string name = command->operands[0]->identifier;
	int parameter_count = std::stoi(command->operands[1]->identifier);

	std::string parent_name = (class_data)->code_memory->parent_name;
	bool parent_exist = parent_name != "";

	if (class_data->code_memory->constructor_function != nullptr) {
		FunctionFrame* constructor_function = new FunctionFrame("constructor");

		std::string type_string = types_to_string_format(machine, constructor_parameters);

		FunctionBundle* constructor_bundle = (class_data->code_memory->constructor_function);

		if (constructor_bundle->code_memories.find(type_string) == constructor_bundle->code_memories.end()) {
			std::cout << "Error! constructor with type of " << name << " does not exist." << std::endl;
			exit(1);
		}

		FunctionCodeMemory* frame = constructor_bundle->code_memories.find(type_string)->second;

		constructor_function->code_memory = frame;
		constructor_function->caller_class = result_memory;

		for (int i = 0; i < parameter_count; i++) {
			Memory* memory = constructor_parameters[i];
			constructor_function->local_memories.insert(std::make_pair(constructor_function->code_memory->parameters[i].first, memory));
		}

		constructor_function->run(machine, command);
	}

	if (parent_exist && class_data->parent_memory == nullptr) {
		std::cout << "Error! qwerasdf" << std::endl;
		exit(1);
	}
}

Memory* create_class(Command* command, Machine* machine, FunctionFrame* frame, const std::string& name, int parameter_count) {
	Memory* result_memory = new Memory(_class, name);
	result_memory->data = new ClassData(name);

	dynamic_cast<ClassData*>(result_memory->data)->code_memory = dynamic_cast<ClassCodeMemory*>(machine->get_class_memory(name));

	return result_memory;
}

Memory* create_object(Command* command, Machine* machine, FunctionFrame* frame, const std::string& name, int parameter_count) {
	Memory* result_memory = new Memory(_object, name);
	result_memory->data = new ObjectData(name);

	dynamic_cast<ObjectData*>(result_memory->data)->code_memory = dynamic_cast<ObjectCodeMemory*>(machine->get_object_memory(name));

	return result_memory;
}

Memory* create_scene(Command* command, Machine* machine, FunctionFrame* frame, const std::string& name, int parameter_count) {
	Memory* result_memory = new Memory(_scene, name);
	result_memory->data = new SceneData(name);

	(dynamic_cast<SceneData*>(result_memory->data))->code_memory = dynamic_cast<SceneCodeMemory*>(machine->get_scene_memory(name));

	return result_memory;
}

void run_command_create(Command* command, Machine* machine, FunctionFrame* frame) {
	std::string name = command->operands[0]->identifier;
	int parameter_count = std::stoi(command->operands[1]->identifier);

	bool class_found = machine->class_code_memories.find(name) != machine->class_code_memories.end();
	bool object_found = machine->object_code_memories.find(name) != machine->object_code_memories.end();
	bool scene_found = machine->scene_code_memories.find(name) != machine->scene_code_memories.end();

	std::vector<Memory*> constructor_parameters = get_constructor_parameters(command, frame);

	Memory* result_memory = nullptr;
	if (class_found) {
		result_memory = create_class(command, machine, frame, name, parameter_count);
		frame->stack->push(result_memory);
	}
	else if (object_found) {
		result_memory = create_object(command, machine, frame, name, parameter_count);
		frame->stack->push(result_memory);
	}
	else if (scene_found) {
		result_memory = create_scene(command, machine, frame, name, parameter_count);
		frame->stack->push(result_memory);
	}
	else {
		std::cout << "Error! cannot create object named : " << name << std::endl;
		exit(1);
	}

	run_initialize_function(machine, result_memory, command);
	run_constructor_function(machine, result_memory, dynamic_cast<ClassData*>(result_memory->data), constructor_parameters, command);
}