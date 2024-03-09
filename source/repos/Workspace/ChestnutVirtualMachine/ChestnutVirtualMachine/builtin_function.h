#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <map>
#include <mutex> 
#include <thread>
#include <chrono>

#include "command.h"
#include "token.h"
#include "frame.h"
#include "builtin_function.h"
#include "builtin_class.h"
#include "memory.h"
#include "engine.h"
#include "SDL.h"

Memory* create_empty_class_memory(const std::string& class_name);
Memory* create_empty_number_memory();
Memory* create_empty_integer_memory();
Memory* create_empty_texture_memory(Machine* machine, const std::string& path);
Data* get_parameter_data(FunctionFrame* frame, const std::string& name);

namespace BUILTIN {
	class BUILTIN_FUNCTION : public FunctionCodeMemory {
	protected:
		bool infinite_parameters = false;
	public:
		BUILTIN_FUNCTION(const std::string name, std::string return_type) : FunctionCodeMemory(name, return_type, "public") {
			type = built_in_function_code_memory;
		}
		bool is_infinite_parameters() const;
		virtual	void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) = 0;
	};

	class PRINT : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		PRINT() : BUILTIN_FUNCTION("print", "void") {
			infinite_parameters = true;
		};
	};

	class INPUT : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		INPUT() : BUILTIN_FUNCTION("input", "void") {
		};
	};

	class INITIALIZE_ENGINE : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		INITIALIZE_ENGINE() : BUILTIN_FUNCTION("initialize_engine", "void") {
		};
	};

	class RUN_ENGINE : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		RUN_ENGINE() : BUILTIN_FUNCTION("start_engine", "void") {
			parameters.push_back(std::make_pair("window", "Window"));
		};
	};

	class CREATE_WINDOW : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		CREATE_WINDOW() : BUILTIN_FUNCTION("create_window", "Window") {
			parameters.push_back(std::make_pair("title", "string"));
			parameters.push_back(std::make_pair("width", "int"));
			parameters.push_back(std::make_pair("height", "int"));
		};
	};

	class CREATE_TEXTURE : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		CREATE_TEXTURE() : BUILTIN_FUNCTION("create_texture", "Window") {
			parameters.push_back(std::make_pair("path", "string"));
		};
	};

	class USE_SHADER : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		USE_SHADER() : BUILTIN_FUNCTION("use_shader", "shader") {
			parameters.push_back(std::make_pair("shader", "shader"));
		};
	};
}