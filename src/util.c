#include "includes/util.h"

int string_to_int(const wchar_t* str) {
	int len = wcslen(str);
	
	int result = 1, digit = 1;
	int i;
	for (i = 0; i < len; i++) {
		if (i == len - 1) {
			if (str[len - i - 1] == '-') {
				result *= -1;
				break;
			}
		}
		
		result = (str[len - i - 1] - '0') * digit;
		digit *= 10;
	}
}

wchar_t* get_working_directory() {
	wchar_t cwd[1024];
	int result = _wgetcwd(cwd, sizeof(cwd) / sizeof(wchar_t));

	if (!result) {
		wprintf("Failed to find cwd.\n");
	}

	return cwd;
}

wchar_t* substr(const wchar_t* str, int s, int e) {
	if (s < 0 || e < s || !str) {
		return NULL;
	}

	int length = e - s + 1;
	wchar_t* result = safe_malloc(sizeof(wchar_t) * (length + 1));;

	wcsncpy_s(result, length + 1, str + s, length);

	result[length] = L'\0';

	return result;
}

void* safe_malloc(size_t size) {
	void* ptr = malloc(size);
	if (!ptr) {
		fprintf(stderr, "malloc failed at %s:%d\n", __FILE__, __LINE__);
	}
	return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
	void* re_ptr = realloc(ptr, size);
	if (!re_ptr) {
		fprintf(stderr, "realloc failed at %s:%d\n", __FILE__, __LINE__);
	}
	return re_ptr;
}

wchar_t* read_file(const wchar_t* file_path) {

	FILE* fp = _wfopen(file_path, "r");
	if (!fp) {
		perror("파일 열기 실패");
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	long byte_len = ftell(fp);
	rewind(fp);

	size_t wchar_estimate = byte_len + 1;
	wchar_t* str = (wchar_t*)malloc(sizeof(wchar_t) * wchar_estimate);
	if (str == NULL) {
		perror("메모리 할당 실패");
		fclose(fp);
		return 1;
	}

	int capacity = 1;
	int size = 0;
	int line_number = 0;

	size_t i = 0;
	while (fgetws(str + i, (int)(wchar_estimate - i), fp)) {
		i = wcslen(str);
	}

	return str;
}

wchar_t* join_string(const wchar_t* str1, const wchar_t* str2) {
	if (!str1) str1 = L"";
	if (!str2) str2 = L"";

	size_t len1 = wcslen(str1);
	size_t len2 = wcslen(str2);

	wchar_t* result = (wchar_t*)malloc((len1 + len2 + 1) * sizeof(wchar_t));
	if (!result) {
		return NULL;
	}

	wcscpy_s(result, len1 + len2 + 1, str1);
	wcscat_s(result, len1 + len2 + 1, str2);

	return result;
}

int is_decimal(wchar_t* str) {
	while (*str != L'\0') {
		if (*str == L'.' || *str == L'f') return 1;
		str++;
	}
	return 0;
}
