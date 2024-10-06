#include "unity.h"
#include "unity_fixture.h"
#include "status.h"
#include "io_handler.h"
#include "lex.h"
#include "parse.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

// None

/****************************************************************************************************
 *	S C A F F O L D I N G
 ****************************************************************************************************/

TEST_GROUP(unit_parse);

TEST_SETUP(unit_parse)
{
	// TEST_ASSERT_EQUAL(STATUS_OK, LEX_init());
}

TEST_TEAR_DOWN(unit_parse)
{
	// TEST_ASSERT_EQUAL(STATUS_OK, LEX_deinit());
}

/****************************************************************************************************
 *	U N I T   T E S T S
 ****************************************************************************************************/

TEST(unit_parse, test_parse_init)
{
	TEST_ASSERT_EQUAL(STATUS_OK, IO_HANDLER_load_source_file("test_files/unit_parse_0.rep"));
	TEST_ASSERT_EQUAL(STATUS_OK, LEX_init());
	LEX_run_fsm();
	PARSE_init();
	PARSE_run_rdp();
}

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

static void run_all_tests(void)
{
	RUN_TEST_CASE(unit_parse, test_parse_init);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_all_tests);
}