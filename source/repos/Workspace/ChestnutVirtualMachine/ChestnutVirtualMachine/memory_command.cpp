#include "memory_command.h"

void run_command_member_store(Command* command, Machine* machine, FunctionFrame* frame) {
	std::string declare_memory_name = command->operands[1]->identifier;

	if (command->type == command_nmstore) {
		if (frame == nullptr) {
			if (frame->caller_class != nullptr) {
				declarate_memory(machine, dynamic_cast<ClassData*>(frame->caller_class->data)->default_memories, command->operands[0]->identifier, frame, declare_memory_name, command);
			}
		}
		else {
			if (frame->scopes.empty()) {
				declarate_memory(machine, frame->local_memories, command->operands[0]->identifier, frame, declare_memory_name, command);
			}
			else {
				declarate_memory(machine, frame->scopes[frame->scopes.size() - 1]->memories, command->operands[0]->identifier, frame, declare_memory_name, command);
			}
		}
	}
	else if (command->type == command_prstore) {
		declarate_memory(machine, dynamic_cast<ClassData*>(frame->caller_class->data)->private_memories, command->operands[0]->identifier, frame, declare_memory_name, command);
	}
	else if (command->type == command_pbstore) {
		declarate_memory(machine, dynamic_cast<ClassData*>(frame->caller_class->data)->public_memories, command->operands[0]->identifier, frame, declare_memory_name, command);
	}
	else if (command->type == command_ptstore) {
		declarate_memory(machine, dynamic_cast<ClassData*>(frame->caller_class->data)->protected_memories, command->operands[0]->identifier, frame, declare_memory_name, command);
	}
}