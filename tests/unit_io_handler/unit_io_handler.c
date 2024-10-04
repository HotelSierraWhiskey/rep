#include "unity.h"
#include "unity_fixture.h"
#include "status.h"
#include "io_handler.h"

/****************************************************************************************************
 *	S C A F F O L D I N G
 ****************************************************************************************************/

TEST_GROUP(unit_io_handler);

TEST_SETUP(unit_io_handler)
{
	// Nothing
}

TEST_TEAR_DOWN(unit_io_handler)
{ 
	UnityConcludeTest(); 
}

/****************************************************************************************************
 *	U N I T   T E S T S
 ****************************************************************************************************/

TEST(unit_io_handler, test_load_source_file_null)
{
	const IO_HANDLER_source_info_t * p_source_info;

	TEST_ASSERT_EQUAL(STATUS_FAILED, IO_HANDLER_load_source_file(NULL));

	p_source_info = IO_HANDLER_get_source_info();
	TEST_ASSERT_EQUAL(NULL, p_source_info->pc_source_buffer);
	TEST_ASSERT_EQUAL(0, p_source_info->u32_size);
}

TEST(unit_io_handler, test_load_source_file_empty_string)
{
	const IO_HANDLER_source_info_t * p_source_info;

	TEST_ASSERT_EQUAL(STATUS_FAILED, IO_HANDLER_load_source_file(""));

	p_source_info = IO_HANDLER_get_source_info();
	TEST_ASSERT_EQUAL(NULL, p_source_info->pc_source_buffer);
	TEST_ASSERT_EQUAL(0, p_source_info->u32_size);
}

TEST(unit_io_handler, test_load_source_file_not_a_rep_file)
{
	const IO_HANDLER_source_info_t * p_source_info;

	TEST_ASSERT_EQUAL(STATUS_INVALID_FILE_ERROR, IO_HANDLER_load_source_file("not_a_rep_file.other"));

	p_source_info = IO_HANDLER_get_source_info();
	TEST_ASSERT_EQUAL(NULL, p_source_info->pc_source_buffer);
	TEST_ASSERT_EQUAL(0, p_source_info->u32_size);
}

TEST(unit_io_handler, test_load_source_file_does_not_exist)
{
	const IO_HANDLER_source_info_t * p_source_info;

	TEST_ASSERT_EQUAL(STATUS_FILE_NOT_FOUND_ERROR, IO_HANDLER_load_source_file("bogus_file.rep"));

	p_source_info = IO_HANDLER_get_source_info();
	TEST_ASSERT_EQUAL(NULL, p_source_info->pc_source_buffer);
	TEST_ASSERT_EQUAL(0, p_source_info->u32_size);
}

TEST(unit_io_handler, test_load_source_file_nominal)
{
	const IO_HANDLER_source_info_t * p_source_info;
	const char * kpc_fname = "unit_io_handler_0.rep";
	FILE * file;
	uint32_t u32_size;
	
	TEST_ASSERT_EQUAL(STATUS_OK, IO_HANDLER_load_source_file(kpc_fname));
	
	file = fopen(kpc_fname, "rb");
	fseek(file, 0, SEEK_END);
	u32_size = ftell(file);
	fclose(file);

	p_source_info = IO_HANDLER_get_source_info();
	TEST_ASSERT(p_source_info->pc_source_buffer);
	TEST_ASSERT_EQUAL(u32_size, p_source_info->u32_size);
}

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

static void run_all_tests(void)
{
	RUN_TEST_CASE(unit_io_handler, test_load_source_file_null);
	RUN_TEST_CASE(unit_io_handler, test_load_source_file_empty_string);
	RUN_TEST_CASE(unit_io_handler, test_load_source_file_not_a_rep_file);
	RUN_TEST_CASE(unit_io_handler, test_load_source_file_does_not_exist);
	RUN_TEST_CASE(unit_io_handler, test_load_source_file_nominal);
}

int main(int argc, const char * argv[])
{
	return UnityMain(argc, argv, run_all_tests);
}