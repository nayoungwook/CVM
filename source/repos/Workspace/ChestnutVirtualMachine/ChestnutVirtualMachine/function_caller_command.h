#pragma once

#include "frame.h"
#include "machine.h"
#include "memory.h"
#include "stack"
#include "code_memory.h"
#include "error.h"
#include "object_create_command.h"

std::vector<Memory*> get_parameter_memories(int parameter_count, FunctionFrame* callee_function_frame);
FunctionFrame* get_class_caller_function_data(Machine* machine, FunctionFrame* caller_frame, std::vector<Memory*> parameters, const std::string& name);
FunctionFrame* get_scene_caller_function_data(Machine* machine, FunctionFrame* caller_frame, std::vector<Memory*> parameters, const std::string& name);
FunctionFrame* get_global_function_data(Machine* machine, FunctionFrame* caller_frame, std::vector<Memory*> parameters, const std::string& name);
FunctionFrame* get_built_function_data(Machine* machine, FunctionFrame* caller_frame, std::vector<Memory*> parameters, const std::string& name);
FunctionFrame* get_function_data(Machine* machine, FunctionFrame* caller_frame, const std::string& name, std::vector<Memory*>& parameters, Command* command);
FunctionFrame* get_constructor_function_data(Machine* machine, FunctionFrame* caller_frame, ClassData* parent_data, std::vector<Memory*> parameters);

void run_function(Machine* machine, FunctionFrame* caller_frame, FunctionFrame* callee_function_frame, const std::string& name,
	std::vector<Memory*>& parameters, Command* command);
void run_command_call(Command* command, Machine* machine, FunctionFrame* frame);