#pragma once

#include <vector>
#include <iostream>
#include "memory.h"

enum operand_type {
	operand_identifier = 1,
	operand_address = 2,
	operand_string = 3,
	operand_number = 4,
	operand_character = 5,
	operand_bool = 6,
};

class Operand {
private:
	operand_type type;
	std::string data;
public:
	operand_type get_type();
	std::string get_data();

	Operand(const std::string, operand_type type);
};

class Stack {
private:
public:
	std::vector<Operand*> operands;

	void push(Operand* memory);
	void pop();
	Operand* peek();
	size_t size();
};