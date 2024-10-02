#include "unity.h"
#include "unity_fixture.h"
#include "io_handler.h"
#include "lex.h"


TEST_GROUP(unit_lex);
TEST_SETUP(unit_lex) { }
TEST_TEAR_DOWN(unit_lex) { }


TEST(unit_lex, test_sanity)
{
	IO_HANDLER_load_source_file("unit_rep_0.rep");
	TEST_ASSERT(1);
}

static void runAllTests(void)
{
	RUN_TEST_CASE(unit_lex, test_sanity);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, runAllTests);
}
