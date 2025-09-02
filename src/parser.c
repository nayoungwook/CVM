#include "includes/parser.h"

Token* consume(TokenType expected_type, const wchar_t* str) {
	Token* tok = pull_token(str);

	if (tok->type != expected_type) {
		wprintf(L"Unexpected token. please check your compiler version.\n");
		abort();
	}

	return tok;
}

void* parse_code_data(const wchar_t* str) {
	Token* first_token = pull_token(str);

	bool is_function_declaration = first_token->type == TokDot;

	if (is_function_declaration) {
		Token* id_token = consume(TokNumberLiteral, str);

		unsigned int id = string_to_int(id_token->str);

		consume(TokLBracket, str);

		while (peek_token(str)->type != TokRBracket) {
			Token* tok = pull_token(str);

			printf("%S : %d\n", tok->str, tok->type);
		}
	}
}
