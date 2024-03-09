#include "stack.h"

void Stack::push(Operand* memory) {
	this->operands.push_back(memory);
}

void Stack::pop() {
	if (this->operands.empty()) {
		std::cout << "Error! no operands to pop" << std::endl;
		exit(1);
	}

	this->operands.erase(this->operands.begin() + (this->operands.size() - 1));
}

operand_type Operand::get_type() {
	return type;
}

std::string Operand::get_data() {
	return data;
}

Operand::Operand(const std::string data, operand_type type) : data(data), type(type) {

}

Operand* Stack::peek() {
	//	std::cout << "[Stack Resource : " << this->operands.size() << " ]" << std::endl;
	if (this->operands.empty()) {
		std::cout << "Error! no operands to peek" << std::endl;
		exit(1);
	}
	return this->operands[this->operands.size() - 1];
}

size_t Stack::size() {
	return this->operands.size();
}