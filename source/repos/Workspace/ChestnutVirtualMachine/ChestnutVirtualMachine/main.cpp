#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "token.h"
#include "machine.h"
#include "frame.h"
#include "memory.h"

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

std::string get_file(const std::string& file_path) {
	std::ifstream openFile(file_path.data());
	std::string result;

	if (openFile.is_open()) {
		std::string line;
		while (getline(openFile, line)) {
			result += line + "\n";
		}
		openFile.close();
	}

	return result;
}

void store_function_frame_into_function_bundle(Machine* machine, FunctionCodeMemory* frame, FunctionBundle* bundle) {
	std::string type_string = types_to_string_format(machine, frame);
	bool type_exist = bundle->code_memories.find(type_string) != bundle->code_memories.end();

	if (!type_exist) {
		bundle->code_memories.insert(std::make_pair(type_string, frame));
	}
	else {
		std::cout << "Error! same name and type of function already exist" << std::endl;
		exit(1);
	}
}

void load_code(Machine* machine, const std::string& file_name, std::vector<Token*>& tokens) {
	machine->add_code_to_memory(file_name, tokens);

	while (!tokens.empty()) {
		CodeMemory* memory = machine->memory_tokens_to_code_memory(tokens);

		if (memory == nullptr) continue;

		if (memory->type == function_code_memory) {
			std::string function_name = dynamic_cast<FunctionCodeMemory*>(memory)->name;
			FunctionCodeMemory* function_code_memory = dynamic_cast<FunctionCodeMemory*>(memory);

			if (((FunctionCodeMemory*)memory)->is_static) {
				if (machine->static_function_code_memories.find(function_name) == machine->static_function_code_memories.end())
					machine->static_function_code_memories.insert(std::make_pair(function_name, new FunctionBundle(function_code_memory->access_modifier)));
				store_function_frame_into_function_bundle(machine, function_code_memory, machine->static_function_code_memories.find(function_name)->second);
			}
			else {
				if (machine->function_code_memories.find(function_name) == machine->function_code_memories.end())
					machine->function_code_memories.insert(std::make_pair(function_name, new FunctionBundle(function_code_memory->access_modifier)));

				store_function_frame_into_function_bundle(machine, function_code_memory, machine->function_code_memories.find(function_name)->second);
			}
		}
		else if (memory->type == class_code_memory) {
			machine->class_code_memories.insert(std::make_pair(
				dynamic_cast<ClassCodeMemory*>(memory)->name, dynamic_cast<ClassCodeMemory*>(memory)));
		}
		else if (memory->type == scene_code_memory) {
			machine->scene_code_memories.insert(std::make_pair(
				dynamic_cast<SceneCodeMemory*>(memory)->name, dynamic_cast<SceneCodeMemory*>(memory)));
		}
		else if (memory->type == object_code_memory) {
			machine->object_code_memories.insert(std::make_pair(
				dynamic_cast<ObjectCodeMemory*>(memory)->name, dynamic_cast<ObjectCodeMemory*>(memory)));
		}
	}
}

int main(int argc, char* argv[]) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Machine* machine = new Machine();

	std::string source = get_file("../../ChestnutCompiler/ChestnutCompiler/test.cir");
	std::vector<Token*> tokens = extract_tokens(source);
	load_code(machine, "main", tokens);

	if (machine->static_function_code_memories.find("main") == machine->static_function_code_memories.end()) {
		std::cout << "Error! cannot find main function (or non-static)" << std::endl;
		exit(1);
	}

	FunctionCodeMemory* main_function = machine->static_function_code_memories["main"]->code_memories.find("")->second;

	main_function->run_code_memory(machine);

	return 0;
}