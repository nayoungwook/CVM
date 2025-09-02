#pragma once

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <io.h>
#include <direct.h>

typedef enum {
	OP_push,
}InstructionTypes;

typedef enum {
	CD_function,
	CD_class,
} CodeDataTypes;

typedef struct _Instruction {
	InstructionTypes type;
} Instruction;

typedef struct _FunctionCodeData {
	unsigned int id;
	unsigned int instruction_size;
	Instruction** instructions;
} FunctioncodeData;

typedef struct _ClassCodeData {
	unsigned int id;
	unsigned int function_size;
	FunctioncodeData** functions;
};
