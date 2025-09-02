#pragma once

#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <io.h>
#include <direct.h>

#define TABLE_SIZE 1024

#define bool char
#define true 1
#define false 0

void* safe_malloc(size_t size);
void* safe_realloc(void* ptr, size_t size);

wchar_t* get_working_directory();
wchar_t* read_file(const wchar_t* path);

wchar_t* substr(const wchar_t* str, int s, int e);

// For strings.
wchar_t* join_string(const wchar_t* str1, const wchar_t* str2);
int is_decimal(wchar_t* str);
int string_to_int(const wchar_t* str);

