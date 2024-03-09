#include "operator_command.h"

void run_command_operator(Command* command, Machine* machine, FunctionFrame* frame) {
	std::pair<Operand*, Operand*> bin = get_lhs_rhs(machine, frame);
	Operand* rhs = bin.first, * lhs = bin.second;

	if (rhs->get_type() == operand_number && lhs->get_type() == operand_number) {
		long double nlhs = std::stold(lhs->get_data());
		long double nrhs = std::stold(rhs->get_data());

		frame->stack->push(new Operand(std::to_string(nlhs + nrhs), operand_number));
	}
	/*
	else if (rhs->get_type() == operand_vector && lhs->get_type() == operand_number) {
		VectorData* vlhs = dynamic_cast<VectorData*>(lhs->data),
			* vrhs = dynamic_cast<VectorData*>(rhs->data);

		Memory* result = new Memory(_vector, "vector");

		size_t size = std::max(vlhs->data_value.size(), vrhs->data_value.size());
		std::vector<Memory*> elements;

		for (int i = 0; i < size; i++) {
			long double nlhs = 0, nrhs = 0;

			if (vlhs->data_value.size() > i) {
				nlhs = get_number_data(vlhs->data_value[i]->data);
			}
			if (vrhs->data_value.size() > i) {
				nrhs = get_number_data(vrhs->data_value[i]->data);
			}
			Memory* element = create_empty_number_memory();
			element->data = operate_number_data(command, command->type, nlhs, nrhs);
			elements.push_back(element);
		}

		result->data = new VectorData(elements);
		frame->stack->push(result);
	}
	*/
	else {
		std::cout << "Cannot operate" << std::endl;
		exit(1);
	}
}

void run_command_compare_operator(Command* command, Machine* machine, FunctionFrame* frame) {
	std::pair<Operand*, Operand*> bin = get_lhs_rhs(machine, frame);
	Operand* rhs = bin.first, * lhs = bin.second;

	if (rhs->get_type() == operand_number && lhs->get_type() == operand_number) {
		bool blhs = (lhs->get_data() == "true");
		bool brhs = (rhs->get_data() == "true");

		if (command->type == command_equal)
			frame->stack->push(new Operand(std::to_string(blhs == brhs), operand_bool));
		else if (command->type == command_not_equal)
			frame->stack->push(new Operand(std::to_string(blhs != brhs), operand_bool));
		else if (command->type == command_lesser)
			frame->stack->push(new Operand(std::to_string(blhs < brhs), operand_bool));
		else if (command->type == command_greater)
			frame->stack->push(new Operand(std::to_string(blhs > brhs), operand_bool));
		else if (command->type == command_eq_greater)
			frame->stack->push(new Operand(std::to_string(blhs >= brhs), operand_bool));
		else if (command->type == command_eq_lesser)
			frame->stack->push(new Operand(std::to_string(blhs <= brhs), operand_bool));

	}
	else {
		std::cout << "Cannot operate" << std::endl;
		exit(1);
	}
}

void run_command_logic_operator(Command* command, Machine* machine, FunctionFrame* frame) {
	std::pair<Operand*, Operand*> bin = get_lhs_rhs(machine, frame);
	Operand* rhs = bin.first, * lhs = bin.second;

	if (!(rhs->get_type() == operand_bool && lhs->get_type() == operand_bool)) {
		std::cout << "Cannot operate " << std::endl;
		exit(1);
	}

	bool blhs = (lhs->get_data() == "true");
	bool brhs = (rhs->get_data() == "true");

	if (command->type == command_or)
		frame->stack->push(new Operand(std::to_string(blhs || brhs), operand_bool));
	else if (command->type == command_and)
		frame->stack->push(new Operand(std::to_string(blhs && brhs), operand_bool));
}