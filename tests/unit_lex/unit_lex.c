#include "unity.h"
#include "unity_fixture.h"
#include "status.h"
#include "io_handler.h"
#include "lex.h"

/****************************************************************************************************
 *	S C A F F O L D I N G
 ****************************************************************************************************/

TEST_GROUP(unit_lex);

TEST_SETUP(unit_lex)
{
	// Nothing
}

TEST_TEAR_DOWN(unit_lex)
{ 
	UnityConcludeTest(); 
}

/****************************************************************************************************
 *	U N I T   T E S T S
 ****************************************************************************************************/

TEST(unit_lex, test_lex_init_deinit)
{
	const IO_HANDLER_source_info_t * p_source_info;
	const LEX_token_list_t * p_token_list = LEX_get_token_list();

	TEST_ASSERT_EQUAL(STATUS_OK, IO_HANDLER_load_source_file("unit_lex_0.rep"));

	p_source_info = IO_HANDLER_get_source_info();
	TEST_ASSERT_NOT_NULL(p_source_info->pc_source_buffer);

	// Init the module, validate
	TEST_ASSERT_EQUAL(STATUS_OK, LEX_init());
	TEST_ASSERT_NOT_NULL(p_token_list->p_tokens);

	// Deinit the module, validate
	TEST_ASSERT_EQUAL(STATUS_OK, LEX_deinit());
	TEST_ASSERT_NULL(p_token_list->p_tokens);
}

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

static void run_all_tests(void)
{
	RUN_TEST_CASE(unit_lex, test_lex_init_deinit);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_all_tests);
}