#pragma once

#include "frame.h"
#include "machine.h"
#include "memory.h"
#include "stack"
#include "code_memory.h"

ClassCodeMemory* get_parent_data(Machine* machine, ClassCodeMemory* code_memory);
void run_command_push(Command* command, Machine* machine, FunctionFrame* frame);
void run_command_array_get(Command* command, Machine* machine, FunctionFrame* frame);
void run_command_store(Command* command, Machine* machine, FunctionFrame* frame);
void run_command_member_store(Command* command, Machine* machine, FunctionFrame* frame);
void run_command_not(Command* command, Machine* machine, FunctionFrame* frame);