#pragma once

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <io.h>
#include <direct.h>

typedef enum {
	OP_push,
}InstructionType;

typedef struct _Instruction {
	InstructionType type;
} Instruction;

typedef struct _FunctionCodeData {
	unsigned int id;
	unsigned int instruction_size;
	Instruction** instructions;
} FunctioncodeData;