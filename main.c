#include "common.h"
#include "status.h"
#include "io_handler.h"
#include "lex.h"
#include "parse.h"
#include "code_gen.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#define MAIN_DBG(fmt, ...)				printf(BOLD("MAIN:\t")fmt, ##__VA_ARGS__)
#define MAIN_GREEN(fmt, ...)			printf(BOLD(BRIGHT_GREEN("MAIN:\t"))fmt, ##__VA_ARGS__)
#define MAIN_WARN(fmt, ...)				printf(BOLD(BRIGHT_YELLOW("MAIN:\t"))fmt, ##__VA_ARGS__)
#define MAIN_ERR(fmt, ...)				printf(BOLD(BRIGHT_RED("MAIN:\t"))fmt, ##__VA_ARGS__)

/****************************************************************************************************
 *	M A I N
 ****************************************************************************************************/

int main(int argc, char** argv)
{
	STATUS_t status;
#ifdef BUILD_DEBUG
	status = IO_HANDLER_load_source_file("debug.rep");
#else
	if (argc != 2)
	{
		MAIN_ERR("Invalid arguments\n");
		return 0;
	}

	const char * fname = argv[1];

	status = IO_HANDLER_load_source_file(fname);
#endif // BUILD_DEBUG

	if (status != STATUS_OK)
	{
		MAIN_ERR("Error (status: %u). Aborting\n", status);
		return 0;
	}

	status = LEX_init();

	if (status != STATUS_OK)
	{
		MAIN_ERR("Error (status: %u). Aborting\n", status);
		return 0;
	}

	LEX_run_fsm();

	PARSE_init();
	PARSE_run_rdp();

	PARSE_tree_list_t * p_tree_list = PARSE_get_tree_list();

	CODE_GEN_traverse_tree(p_tree_list->trees[0]);
}