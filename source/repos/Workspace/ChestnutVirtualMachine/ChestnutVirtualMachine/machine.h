#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

#include <map>

#include "token.h"
#include "memory.h"
#include "machine.h"
#include "command.h"
#include "code_memory.h"
#include "builtin_function.h"
#include "builtin_math.h"
#include "builtin_class.h"

using namespace BUILTIN;

class Machine {
private:
public:
	Machine();
	virtual ~Machine() = default;

	std::map<std::string, std::vector<Token*>> memory_code;

	std::vector<Frame*> function_frames;

	std::map<std::string, FunctionBundle*> static_function_code_memories;

	std::map<std::string, ClassCodeMemory*> class_code_memories;
	std::map<std::string, ObjectCodeMemory*> object_code_memories;
	std::map<std::string, SceneCodeMemory*> scene_code_memories;
	std::map<std::string, FunctionBundle*> function_code_memories;
	std::map<std::string, FunctionBundle*> built_in_function_code_memories;

	std::map<std::string, int> label_table;

	std::map<std::string, Scope*> stack_area;

	std::map<std::string, Memory*> global_memories;
	SDL_GLContext context;
	ShaderData* default_shader;

	ShaderData* current_shader;

	void add_code_to_memory(const std::string& name, std::vector<Token*> tokens);
	CodeMemory* memory_tokens_to_code_memory(std::vector<Token*>& memory_tokens);
	Token* get_token(std::vector<Token*>& tokens);
	void remove_token(std::vector<Token*>& tokens);
	std::vector<Token*> get_body_tokens(std::vector<Token*>& tokens);
	std::vector<Token*>& load_code_memory(const std::string& name);
	std::vector<Command*> get_commands(std::vector<Token*> body_tokens, FunctionCodeMemory* function_code_memory);

	SceneCodeMemory* get_scene_memory(const std::string& name);
	ClassCodeMemory* get_class_memory(const std::string& name);
	ObjectCodeMemory* get_object_memory(const std::string& name);
	FunctionCodeMemory* get_member_function_memory(ClassCodeMemory* frame, std::string const& name, std::vector<Memory*>& parameter_memories);
};

std::string to_upper_all(const std::string& str);
void store_function(Machine* machine, FunctionCodeMemory* func_memory, ClassCodeMemory* class_memory);
