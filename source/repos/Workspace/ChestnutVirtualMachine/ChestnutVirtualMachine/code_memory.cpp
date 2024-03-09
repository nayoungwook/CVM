#include "code_memory.h"

void FunctionCodeMemory::run_code_memory(Machine* machine) {
	FunctionFrame* frame = new FunctionFrame(name);
	machine->function_frames.push_back(frame);
	frame->code_memory = this;
	frame->run(machine, nullptr);
}
