#include "basic_command.h"

void run_command_push(Command* command, Machine* machine, FunctionFrame* frame) {
	std::string operand = command->operands[0]->identifier;

	bool _is_digit = is_digit(operand) || ((operand[0] == '-' || operand[0] == '+') && is_digit(operand.substr(1, operand.size() - 1)));
	bool _is_identifier = !_is_digit;
	bool _is_string_literal = _is_identifier && operand[0] == '\"';
	bool _is_character = _is_identifier && operand[0] == '\'' && operand[operand.size() - 1] == '\'';
	bool _is_bool = operand == "true" || operand == "false";

	operand_type type = operand_identifier;

	if (_is_digit)
		type = operand_number;
	else if (_is_string_literal)
		type = operand_string;
	else if (_is_character)
		type = operand_character;
	else if (_is_bool)
		type = operand_bool;

	frame->stack->push(new Operand(operand, type));
}

ClassCodeMemory* get_parent_data(Machine* machine, ClassCodeMemory* code_memory) {
	if (code_memory->parent_name == "") {
		return code_memory;
	}
	else {
		return get_parent_data(machine, machine->get_class_memory(code_memory->parent_name));
	}
}

void run_command_store(Command* command, Machine* machine, FunctionFrame* frame) {
	Memory* rhs = frame->stack->peek();
	frame->stack->pop();
	Memory* lhs = frame->stack->peek();
	frame->stack->pop();

	if (!(rhs->data->root_type == lhs->data->root_type || rhs->type == lhs->type)) {
		std::cout << command->line_number << std::endl;
		std::cout << "Error! different types." << std::endl;
		exit(1);
	}

	if (rhs->type == _class && lhs->type == _class || rhs->type == _object && lhs->type == _object) {
		std::string _rhs, _lhs;

		if (rhs->type == _class && lhs->type == _class)
			_rhs = get_parent_data(machine, machine->get_class_memory(rhs->declaration_name))->name,
			_lhs = get_parent_data(machine, machine->get_class_memory(lhs->declaration_name))->name;

		if (rhs->type == _object && lhs->type == _object)
			_rhs = get_parent_data(machine, machine->get_object_memory(rhs->declaration_name))->name,
			_lhs = get_parent_data(machine, machine->get_object_memory(lhs->declaration_name))->name;

		if (_lhs != _rhs) {
			std::cout << "Error! class type of lhs does not match with rhs | " << _lhs << " : " << _rhs << std::endl;
			exit(1);
		}
	}

	Data* changed_data = nullptr;

	if (lhs->data->root_type == _class)
		changed_data = change_type_of_class(machine, rhs, dynamic_cast<ClassData*>(lhs->data)->code_memory->name, lhs);
	else if (lhs->data->root_type == _object)
		changed_data = change_type_of_class(machine, rhs, dynamic_cast<ObjectData*>(lhs->data)->code_memory->name, lhs);
	else
		changed_data = change_data(rhs, lhs->type);

	(*lhs).data = changed_data;
}

void run_command_array_get(Command* command, Machine* machine, FunctionFrame* frame) {
	Memory* index = frame->stack->peek();
	frame->stack->pop();
	Memory* memory = frame->stack->peek();
	frame->stack->pop();

	if (memory->data->root_type == _array) {
		if (index->data->root_type == _number) {
			ArrayData* array_data = dynamic_cast<ArrayData*>(memory->data);
			long double index_data = get_number_data(index->data);

			if (index_data != (int)index_data) {
				std::cout << "Error! index is not integer." << std::endl;
			}

			if (array_data->data_value.size() <= index_data) {
				std::cout << "Error! array out of index" << std::endl;
				exit(1);
			}

			frame->stack->push(
				(array_data)->data_value[
					(int)(index_data)
				]);
		}
		else {
			std::cout << "Error! index is not number." << std::endl;
			exit(1);
		}
	}
	else {
		std::cout << "Error! not array. " << memory->data->root_type << std::endl;
		exit(1);
	}
}

void run_command_not(Command* command, Machine* machine, FunctionFrame* frame) {
	Operand* operand = frame->stack->peek();
	frame->stack->pop();

	if (operand->get_type() == operand_bool) {
		std::string data = operand->get_data() == "true" ? "false" : "true";
		frame->stack->push(new Operand(data, operand_bool));
	}
	else {
		std::cout << "Error! current memory is not bool type." << std::endl;
		exit(1);
	}
}