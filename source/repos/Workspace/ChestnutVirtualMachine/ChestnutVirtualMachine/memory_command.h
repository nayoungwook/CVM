#pragma once

#include "frame.h"
#include "machine.h"
#include "memory.h"
#include "stack"
#include "code_memory.h"

void run_command_member_store(Command* command, Machine* machine, FunctionFrame* frame);