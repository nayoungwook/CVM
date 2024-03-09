#pragma once

#include "machine.h"
#include "frame.h"
#include "command.h"
#include "builtin_math.h"
#include "command.h"
#include "builtin_function.h"

namespace BUILTIN {
	class SIN : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		SIN() : BUILTIN_FUNCTION("sin", "number") {
			parameters.push_back(std::make_pair("x", "number"));
		};
	};
	class COS : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		COS() : BUILTIN_FUNCTION("cos", "number") {
			parameters.push_back(std::make_pair("x", "number"));
		};
	};

	class TAN : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		TAN() : BUILTIN_FUNCTION("tan", "number") {
			parameters.push_back(std::make_pair("x", "number"));
		};
	};

	class SQRT : public BUILTIN_FUNCTION {
	private:
	public:
		virtual void run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command);
		SQRT() : BUILTIN_FUNCTION("sqrt", "number") {
			parameters.push_back(std::make_pair("x", "number"));
		};
	};
}