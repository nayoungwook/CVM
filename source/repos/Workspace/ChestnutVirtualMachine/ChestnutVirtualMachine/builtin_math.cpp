#include "builtin_math.h"

void BUILTIN::SIN::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	Memory* result = create_empty_number_memory();

	double parameter = get_number_data(frame->local_memories["x"]->data);
	dynamic_cast<NumberData*>(result->data)->data_value = sin(parameter);

	caller->stack->push(result);
}

void BUILTIN::COS::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	Memory* result = create_empty_number_memory();

	double parameter = get_number_data(frame->local_memories["x"]->data);
	dynamic_cast<NumberData*>(result->data)->data_value = cos(parameter);

	caller->stack->push(result);
}

void BUILTIN::TAN::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	Memory* result = create_empty_number_memory();

	double parameter = get_number_data(frame->local_memories["x"]->data);
	dynamic_cast<NumberData*>(result->data)->data_value = tan(parameter);

	caller->stack->push(result);
}

void BUILTIN::SQRT::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	Memory* result = create_empty_number_memory();

	double parameter = get_number_data(frame->local_memories["x"]->data);
	dynamic_cast<NumberData*>(result->data)->data_value = sqrt(parameter);

	caller->stack->push(result);
}