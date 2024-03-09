#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <memory>
#include <minmax.h>
#include <stack>

#include "command.h"
#include "stack.h"
#include "scope.h"
#include "builtin_function.h"
#include "memory.h"
#include "frame.h"
#include "machine.h"
#include "error.h"
#include "commands.h"

bool is_digit(std::string str);

class Frame {
private:
public:
	virtual ~Frame() {};
	Frame() {};
	Stack* stack = new Stack();
};

class FunctionFrame : public Frame {
private:
public:
	std::string name;
	FunctionCodeMemory* code_memory = nullptr;
	Memory* caller_class = nullptr;
	FunctionFrame* caller_function = nullptr;
	std::map<std::string, Memory*> local_memories;
	std::vector<Scope*> scopes;

	virtual void run(Machine* machine, Command* command);
	~FunctionFrame() {
	}

	FunctionFrame(std::string name) : name(name) {}
};

class BuiltInFunctionFrame : public FunctionFrame {
private:
public:
	BUILTIN::BUILTIN_FUNCTION* built_in_code_memory = nullptr;
	void run(Machine* machine, FunctionFrame* caller, Command* command);
	BuiltInFunctionFrame(std::string name) : FunctionFrame(name) {};
};

std::pair<Operand*, Operand*> get_lhs_rhs(Machine* machine, FunctionFrame* frame);
Data* change_data(Memory* memory, data_type type);
Memory* operand_to_memory(const std::string& operand, Machine* machine, FunctionFrame* frame, Command* command);
Memory* load_memory(Machine* machine, FunctionFrame* frame, const std::string& name, Command* command);
std::string get_memory_data_print_value(Memory* mem, Machine* machine, FunctionFrame* caller);
inline void find_label(Machine* frame, const std::string& index, int& i);
void declarate_memory(Machine* machine, std::map<std::string, Memory*>& memory_area, const std::string& name, FunctionFrame* frame, const std::string& memory_name, Command* command);
void run_initialize_function(Machine* machine, Memory* memory, Command* command);
long double get_number_data(Data* data);
Data* change_type_of_class(Machine* machine, Memory* memory, const std::string& name, Memory* target_memory);
BuiltInFunctionFrame* exchange_bulitin_function_frame(FunctionFrame* frame);