#pragma once
#include "frame.h"
#include "machine.h"
#include "memory.h"

#include "builtin_function.h"
#include <set>
#include "commands.h"
#include "error.h"

#include "gl/glew.h"
#include "gl/GL.h"
#include "SDL_opengl.h"

#include "SDL.h"
#include "SDL_image.h"
#include "stb_image.h"

void class_load_call(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame, std::vector<Memory*>& parameters);
void object_load_call(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame, std::vector<Memory*>& parameters);
Memory* class_load_identifier(Memory* peek_memory, const std::string& name, Command* command, Machine* machine, FunctionFrame* frame);
Memory* object_load_identifier(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame);
Data* expect_and_get_parameter_data(Machine* machine, Command* command, FunctionFrame* frame, std::vector<Memory*>& parameters, int index, data_type expected_type);

void array_load_call(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame, std::vector<Memory*>& parameters);
void vector_load_identifier(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame);
void texture_load_call(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame, std::vector<Memory*>& parameters);

void run_command_load_call(Command* command, Machine* machine, FunctionFrame* frame);
void run_command_load_identifier(Command* command, Machine* machine, FunctionFrame* frame);
