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
	// Nothing
}

TEST_TEAR_DOWN(unit_parse)
{
	// Nothing
}

/****************************************************************************************************
 *	U N I T   T E S T S
 ****************************************************************************************************/

TEST(unit_parse, test_parse_init)
{
	PARSE_tree_container_t trees;

	TEST_ASSERT_EQUAL(STATUS_OK, IO_HANDLER_load_source_file("test_files/unit_parse_0.rep"));
	TEST_ASSERT_EQUAL(STATUS_OK, LEX_init());
	LEX_run_fsm();
	PARSE_init();
	PARSE_run_rdp();

	trees = PARSE_get_all_trees();

	// TODO: Are these always equal?
	TEST_ASSERT_EQUAL(PARSE_get_num_trees(), LEX_get_num_statements());

	TEST_ASSERT_NOT_NULL(trees);

	for (uint32_t i = 0; i < LEX_get_num_statements(); i++)
	{
		PARSE_traverse_tree(trees[i], 0, PARSE_NODE_SIDE_ROOT);
	}
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