#pragma once

#include "frame.h"
#include "machine.h"
#include "memory.h"
#include "stack"
#include "code_memory.h"

void run_command_create(Command* command, Machine* machine, FunctionFrame* frame);
Memory* create_class(Command* command, Machine* machine, FunctionFrame* frame, const std::string& name, int parameter_count);
Memory* create_object(Command* command, Machine* machine, FunctionFrame* frame, const std::string& name, int parameter_count);
