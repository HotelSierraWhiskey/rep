#include "unity.h"
#include "unity_fixture.h"
#include "status.h"
#include "io_handler.h"


TEST_GROUP(unit_io_handler);
TEST_SETUP(unit_io_handler) { }
TEST_TEAR_DOWN(unit_io_handler) { }


TEST(unit_io_handler, test_load_source_file)
{
	TEST_ASSERT_EQUAL(STATUS_FAILED, IO_HANDLER_load_source_file(NULL));
	TEST_ASSERT_EQUAL(STATUS_FAILED, IO_HANDLER_load_source_file(""));
	TEST_ASSERT_EQUAL(STATUS_INVALID_FILE_ERROR, IO_HANDLER_load_source_file("not_a_rep_file.other"));
	TEST_ASSERT_EQUAL(STATUS_FILE_NOT_FOUND_ERROR, IO_HANDLER_load_source_file("bogus_file.rep"));
}

static void runAllTests(void)
{
	RUN_TEST_CASE(unit_io_handler, test_load_source_file);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, runAllTests);
}