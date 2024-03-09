#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

#include "command.h"
#include "token.h"
#include "frame.h"
#include "machine.h"

class Machine;
class FunctionFrame;

enum code_memory_type {
	default_code_memory = 0, function_code_memory = 1, class_code_memory = 2,
	initialize_function_code_memory = 3, built_in_function_code_memory = 4,
	constructor_function_code_memory = 5, scene_code_memory = 6, object_code_memory = 7,
};

class CodeMemory {
private:
public:
	std::vector<Command*> commands;
	code_memory_type type;

	virtual ~CodeMemory() {};
	CodeMemory() {
		type = default_code_memory;
	}
};

class FunctionCodeMemory : public CodeMemory {
public:
	std::string name;
	std::string return_type, access_modifier;
	std::vector<std::pair<std::string, std::string>> parameters;
	void run_code_memory(Machine* machine);
	bool is_static = false;

	FunctionCodeMemory(const std::string& name, const std::string& return_type, const std::string& access_modifier) : name(name), return_type(return_type), access_modifier(access_modifier) {
		type = function_code_memory;
	}
};

class FunctionBundle {
private:
public:
	std::map<std::string, FunctionCodeMemory*> code_memories;
	std::string access_modifier;
	FunctionBundle(const std::string& access_modifier) : access_modifier(access_modifier) {}
	FunctionBundle(std::pair<std::string, FunctionCodeMemory*> code_memory, const std::string& access_modifier) : access_modifier(access_modifier) { code_memories.insert(code_memory); }
};

class InitializeFunctionCodeMemory : public FunctionCodeMemory {
public:
	InitializeFunctionCodeMemory(const std::string& name, const std::string& return_type, const std::string& access_modifier)
		: FunctionCodeMemory(name, return_type, access_modifier) {
		type = initialize_function_code_memory;
	}
};

class ConstructorFunctionCodeMemory : public FunctionCodeMemory {
public:
	ConstructorFunctionCodeMemory(const std::string& name, const std::string& return_type, const std::string& access_modifier)
		: FunctionCodeMemory(name, return_type, access_modifier) {
		type = constructor_function_code_memory;
	}
};

class ClassCodeMemory : public CodeMemory {
public:
	std::string name;
	std::string parent_name = "";

	std::map<std::string, FunctionBundle*> member_functions;
	InitializeFunctionCodeMemory* initialize_function = nullptr;
	FunctionBundle* constructor_function = nullptr;

	ClassCodeMemory(const std::string& name) : name(name) {
		type = class_code_memory;
	};
};

class ObjectCodeMemory : public ClassCodeMemory {
public:
	ObjectCodeMemory(const std::string& name) : ClassCodeMemory(name) {
		type = object_code_memory;
	};
};

class SceneCodeMemory : public ClassCodeMemory {
public:
	SceneCodeMemory(const std::string& name) : ClassCodeMemory(name) {
		type = scene_code_memory;
	};
};
