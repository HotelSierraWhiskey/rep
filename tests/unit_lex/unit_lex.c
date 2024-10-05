#include "unity.h"
#include "unity_fixture.h"
#include "status.h"
#include "io_handler.h"
#include "lex.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#define ASSERT_CURRENT_TOKEN_VALID(expected_lexeme, expected_type, token, counter) \
	do \
	{\
		TEST_ASSERT_EQUAL_STRING(expected_lexeme, token[counter].pc_lexeme); \
		TEST_ASSERT_EQUAL(expected_type, token[counter].type); \
		(counter)++; \
	} \
	while(0)

/****************************************************************************************************
 *	S C A F F O L D I N G
 ****************************************************************************************************/

TEST_GROUP(unit_lex);

TEST_SETUP(unit_lex)
{
	// Init the module, validate
	const LEX_token_list_t * kp_token_list = LEX_get_token_list();
	printf("\n");
	TEST_ASSERT_EQUAL(STATUS_OK, LEX_init());
	TEST_ASSERT_NOT_NULL(kp_token_list->p_tokens);
}

TEST_TEAR_DOWN(unit_lex)
{
	// Deinit the module, validate
	const LEX_token_list_t * kp_token_list = LEX_get_token_list();
	printf("\n");
	TEST_ASSERT_EQUAL(STATUS_OK, LEX_deinit());
	TEST_ASSERT_NULL(kp_token_list->p_tokens);
	UnityConcludeTest(); 
}

/****************************************************************************************************
 *	U N I T   T E S T S
 ****************************************************************************************************/

TEST(unit_lex, test_single_expression_nominal)
{
	const LEX_token_list_t * kp_token_list;
	uint32_t u32_tokens_checked = 0;
	const uint32_t ku32_tokens_expected = 4;
	const uint32_t ku32_statements_expected = 1;

	// 	test file reads:
	//		2 + 2;
	TEST_ASSERT_EQUAL(STATUS_OK, IO_HANDLER_load_source_file("test_files/unit_lex_0.rep"));

	LEX_run_fsm();

	kp_token_list = LEX_get_token_list();

	TEST_ASSERT_EQUAL(ku32_statements_expected, LEX_get_num_statements());
	TEST_ASSERT_EQUAL(ku32_tokens_expected, kp_token_list->u32_num_tokens);

	ASSERT_CURRENT_TOKEN_VALID("2", LEX_TOKEN_TYPE_INT_LITERAL, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("+", LEX_TOKEN_TYPE_OP_ADD, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("2", LEX_TOKEN_TYPE_INT_LITERAL, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	TEST_ASSERT_EQUAL(ku32_tokens_expected, u32_tokens_checked);
}

TEST(unit_lex, test_multiple_expression_nominal)
{
	const LEX_token_list_t * kp_token_list;
	uint32_t u32_tokens_checked = 0;
	const uint32_t ku32_tokens_expected = 8;
	const uint32_t ku32_statements_expected = 2;

	// 	test file reads:
	//		2 + 2;
	//		3 * 3;
	TEST_ASSERT_EQUAL(STATUS_OK, IO_HANDLER_load_source_file("test_files/unit_lex_1.rep"));

	LEX_run_fsm();

	kp_token_list = LEX_get_token_list();

	TEST_ASSERT_EQUAL(ku32_statements_expected, LEX_get_num_statements());
	TEST_ASSERT_EQUAL(ku32_tokens_expected, kp_token_list->u32_num_tokens);

	ASSERT_CURRENT_TOKEN_VALID("2", LEX_TOKEN_TYPE_INT_LITERAL, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("+", LEX_TOKEN_TYPE_OP_ADD, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("2", LEX_TOKEN_TYPE_INT_LITERAL, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	ASSERT_CURRENT_TOKEN_VALID("3", LEX_TOKEN_TYPE_INT_LITERAL, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("*", LEX_TOKEN_TYPE_OP_MULTIPLY, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("3", LEX_TOKEN_TYPE_INT_LITERAL, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	TEST_ASSERT_EQUAL(ku32_tokens_expected, u32_tokens_checked);
}

TEST(unit_lex, test_excessive_delims)
{
	const LEX_token_list_t * kp_token_list;
	uint32_t u32_tokens_checked = 0;
	const uint32_t ku32_tokens_expected = 9;
	const uint32_t ku32_statements_expected = 6;

	// 	test file reads:
	//		;;;1 + 1;;;
	TEST_ASSERT_EQUAL(STATUS_OK, IO_HANDLER_load_source_file("test_files/unit_lex_2.rep"));

	LEX_run_fsm();

	kp_token_list = LEX_get_token_list();

	TEST_ASSERT_EQUAL(ku32_statements_expected, LEX_get_num_statements());
	TEST_ASSERT_EQUAL(ku32_tokens_expected, kp_token_list->u32_num_tokens);

	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("1", LEX_TOKEN_TYPE_INT_LITERAL, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("+", LEX_TOKEN_TYPE_OP_ADD, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("1", LEX_TOKEN_TYPE_INT_LITERAL, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	TEST_ASSERT_EQUAL(ku32_tokens_expected, u32_tokens_checked);
}

TEST(unit_lex, test_identifier_tokenization)
{
	const LEX_token_list_t * kp_token_list;
	uint32_t u32_tokens_checked = 0;
	const uint32_t ku32_tokens_expected = 15;
	const uint32_t ku32_statements_expected = 6;

	// 	test file reads:
	//		123abc;
	//		abc123;
	//		1a2b3c;
	//		a1b2c3;
	//		a?@#$^;
	//		_abcd_;
	TEST_ASSERT_EQUAL(STATUS_OK, IO_HANDLER_load_source_file("test_files/unit_lex_3.rep"));

	LEX_run_fsm();

	kp_token_list = LEX_get_token_list();

	TEST_ASSERT_EQUAL(ku32_statements_expected, LEX_get_num_statements());
	TEST_ASSERT_EQUAL(ku32_tokens_expected, kp_token_list->u32_num_tokens);

	// This is not a valid identifier (starts with a number)
	ASSERT_CURRENT_TOKEN_VALID("123abc", LEX_TOKEN_TYPE_UNKNOWN, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	// This is a valid identifier
	ASSERT_CURRENT_TOKEN_VALID("abc123", LEX_TOKEN_TYPE_IDENTIFIER, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	// This is not a valid identifier (also starts with a number)
	ASSERT_CURRENT_TOKEN_VALID("1a2b3c", LEX_TOKEN_TYPE_UNKNOWN, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	// This is a valid identifier
	ASSERT_CURRENT_TOKEN_VALID("a1b2c3", LEX_TOKEN_TYPE_IDENTIFIER, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	// NOTE:
	// Based on the source file, you might assume we're getting an invalid token of a?@#$^. Not so.
	// Refer to the state machine transitions for insight on why these are split up
	ASSERT_CURRENT_TOKEN_VALID("a?@", LEX_TOKEN_TYPE_UNKNOWN, kp_token_list->p_tokens, u32_tokens_checked);

	ASSERT_CURRENT_TOKEN_VALID("#", LEX_TOKEN_TYPE_UNKNOWN, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("$", LEX_TOKEN_TYPE_UNKNOWN, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID("^", LEX_TOKEN_TYPE_UNKNOWN, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	// This is a valid identifier
	ASSERT_CURRENT_TOKEN_VALID("_abcd_", LEX_TOKEN_TYPE_IDENTIFIER, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	TEST_ASSERT_EQUAL(ku32_tokens_expected, u32_tokens_checked);
}

TEST(unit_lex, test_lexeme_max_size)
{
	const LEX_token_list_t * kp_token_list;
	uint32_t u32_tokens_checked = 0;
	const uint32_t ku32_tokens_expected = 4;
	const uint32_t ku32_statements_expected = 2;

	TEST_ASSERT_EQUAL(STATUS_OK, IO_HANDLER_load_source_file("test_files/unit_lex_4.rep"));

	LEX_run_fsm();

	kp_token_list = LEX_get_token_list();

	TEST_ASSERT_EQUAL(ku32_statements_expected, LEX_get_num_statements());
	TEST_ASSERT_EQUAL(ku32_tokens_expected, kp_token_list->u32_num_tokens);

	// Plus one in this comparison for the null terminator
	TEST_ASSERT_EQUAL(LEX_MAX_LEXEME_SIZE, strlen(kp_token_list->p_tokens[0].pc_lexeme));

	ASSERT_CURRENT_TOKEN_VALID("this_is_a_long_variable_name_this_is_a_long_variable_name_this_is_a_long_variable_name_this_is_a_long_variable_name_this_is_a_long_variable_name_this_is_a_long_variable_name_this_is_a_long_variable_name_this_is_a_long_variable_name_this_is_a_long_variable", LEX_TOKEN_TYPE_IDENTIFIER, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	ASSERT_CURRENT_TOKEN_VALID(LEX_MAX_LEXEME_SIZE_EXCEEDED, LEX_TOKEN_TYPE_UNKNOWN, kp_token_list->p_tokens, u32_tokens_checked);
	ASSERT_CURRENT_TOKEN_VALID(";", LEX_TOKEN_TYPE_DELIM, kp_token_list->p_tokens, u32_tokens_checked);

	TEST_ASSERT_EQUAL(ku32_tokens_expected, u32_tokens_checked);
}

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

static void run_all_tests(void)
{
	RUN_TEST_CASE(unit_lex, test_single_expression_nominal);
	RUN_TEST_CASE(unit_lex, test_multiple_expression_nominal);
	RUN_TEST_CASE(unit_lex, test_excessive_delims);
	RUN_TEST_CASE(unit_lex, test_identifier_tokenization);
	RUN_TEST_CASE(unit_lex, test_lexeme_max_size);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_all_tests);
}
