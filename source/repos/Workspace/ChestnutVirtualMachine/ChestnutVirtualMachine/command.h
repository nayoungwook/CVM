#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "token.h"

enum command_type {
	command_null = 0,
	command_push = 1,
	command_store = 2,
	command_nmstore = 3,
	command_prstore = 4,
	command_pbstore = 5,
	command_ptstore = 6,
	command_add = 7,
	command_sub = 8,
	command_mult = 9,
	command_div = 10,
	command_call = 11,
	command_create = 12,
	command_return = 13,
	command_load_call = 14,
	command_load_identifier = 15,
	command_equal = 16,
	command_not_equal = 17,
	command_lesser = 18,
	command_greater = 19,
	command_eq_lesser = 20,
	command_eq_greater = 21,
	command_or = 22,
	command_and = 23,
	command_if = 24,
	command_goto = 25,
	command_for = 26,
	command_label = 27,
	command_mod = 28,
	command_pow = 29,
	command_array_get = 30,
	command_not = 31,
	command_create_scope = 32,
	command_end_scope = 33,
	command_array = 34,
	command_vector = 35,
	command_incre = 36,
	command_decre = 37,
};

class Command {
private:
public:
	command_type type = command_null;
	std::vector<Token*> operands;
	int line_number = 0;
	bool refer = false;
};