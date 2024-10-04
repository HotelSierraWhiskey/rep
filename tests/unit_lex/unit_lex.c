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

TEST(unit_lex, test_run_fsm_single_expression_nominal)
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

TEST(unit_lex, test_run_fsm_multiple_expression_nominal)
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

TEST(unit_lex, test_run_fsm_excessive_delims)
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

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

static void run_all_tests(void)
{
	RUN_TEST_CASE(unit_lex, test_run_fsm_single_expression_nominal);
	RUN_TEST_CASE(unit_lex, test_run_fsm_multiple_expression_nominal);
	RUN_TEST_CASE(unit_lex, test_run_fsm_excessive_delims);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_all_tests);
}