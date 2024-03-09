#pragma once

#include "frame.h"
#include "machine.h"
#include "memory.h"
#include "stack"
#include "code_memory.h"
#include "error.h"

void run_command_operator(Command* command, Machine* machine, FunctionFrame* frame);
void run_command_compare_operator(Command* command, Machine* machine, FunctionFrame* frame);
void run_command_logic_operator(Command* command, Machine* machine, FunctionFrame* frame);
