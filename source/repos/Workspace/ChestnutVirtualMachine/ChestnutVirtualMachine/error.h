#pragma once

#include <map>
#include <iostream>
#include <string>
#include "error.h"

enum class error_codes {
	unknown = 0,
	not_defined = 1,
	already_exist = 2,
	function_not_defined = 3,
	unknown_type = 4,
	failed_to_initialize_opengl = 5,
	division_by_zero = 6,
	failed_to_load_from_identifier = 7,
	access_modifier = 8,
	failed_to_find_this = 9,
	failed_to_find_super = 10,
	function_must_return_value = 11,
};

class Error {
private:
public:
	error_codes error_code;
	int line_number = 0;
	Error(error_codes error_code, int line_number) : error_code(error_code), line_number(line_number) {
	};
	void throw_error(const std::string& message, int line_number);
};

class VariableNotFoundError : public Error {
public:
	VariableNotFoundError(const std::string& identifier, int line_number) : Error(error_codes::not_defined, line_number) {
		throw_error("Variable named \"" + identifier + "\" not found in this area.", line_number);
	}
};

class VariableAlreadyExistError : public Error {
public:
	VariableAlreadyExistError(const std::string& identifier, int line_number) : Error(error_codes::already_exist, line_number) {
		throw_error("Variable named \"" + identifier + "\" already exist in this scope.", line_number);
	}
};

class FunctionNotFoundError : public Error {
public:
	FunctionNotFoundError(const std::string& identifier, const std::string& type_string, int line_number) : Error(error_codes::function_not_defined, line_number) {
		throw_error("Function named \"" + identifier + "\" with parameter \"" + type_string + " \"", line_number);
	}
};

class AccessModifierError : public Error {
public:
	AccessModifierError(const std::string& identifier, int line_number) : Error(error_codes::access_modifier, line_number) {
		throw_error("Cannot access " + identifier + " at this level.", line_number);
	}
};

class UnknownTypeError : public Error {
public:
	UnknownTypeError(const std::string& type, int line_number) : Error(error_codes::unknown_type, line_number) {
		throw_error("Can\'t find type : \"" + type + "\"", line_number);
	}
};

class FailedToInitializeOpenGLError : public Error {
public:
	FailedToInitializeOpenGLError(int line_number) : Error(error_codes::failed_to_initialize_opengl, line_number) {
		throw_error("Failed to initialize opengl.\n please re-install chestnut-language or checkout library settings.", line_number);
	}
};

class DivisionByZeroError : public Error {
public:
	DivisionByZeroError(int line_number) : Error(error_codes::division_by_zero, line_number) {
		throw_error("Division By zero. Please check divisor of expression.", line_number);
	}
};

class FailedToLoadFromIdentifierError : public Error {
public:
	FailedToLoadFromIdentifierError(const std::string& type, int line_number) : Error(error_codes::failed_to_load_from_identifier, line_number) {
		throw_error("Failed to load identifier or function of : " + type, line_number);
	}
};

class FailedToFindThisError : public Error {
public:
	FailedToFindThisError(int line_number) : Error(error_codes::failed_to_find_this, line_number) {
		throw_error("Failed to find \'this\' identifier in this function.", line_number);
	}
};

class FailedToFindSuperError : public Error {
public:
	FailedToFindSuperError(int line_number) : Error(error_codes::failed_to_find_super, line_number) {
		throw_error("Failed to find \'super\' identifier in this function.", line_number);
	}
};

class FailedToStoreVariable : public Error {
public:
	FailedToStoreVariable(const std::string& lhs, const std::string& rhs, int line_number) : Error(error_codes::failed_to_find_super, line_number) {
		throw_error("Failed to store " + rhs + " into " + lhs, line_number);
	}
};

class FunctionMustReturnValueError : public Error {
public:
	FunctionMustReturnValueError(const std::string& name, const std::string& return_type, int line_number) : Error(error_codes::function_must_return_value, line_number) {
		throw_error("Function \'" + name + "\' must return \'" + return_type + "\' \nCheck if there is a case for which this function does not return a value.", line_number);
	}
};