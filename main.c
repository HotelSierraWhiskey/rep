#include "common.h"
#include "status.h"
#include "io_handler.h"
#include "lex.h"
#include "parse.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#define MAIN_DBG(fmt, ...)	printf(BOLD("MAIN:\t")fmt, ##__VA_ARGS__)

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
		MAIN_DBG(BOLD(BRIGHT_RED("Invalid arguments\n")));
		return 0;
	}

	const char * fname = argv[1];

	status = IO_HANDLER_load_source_file(fname);
#endif // BUILD_DEBUG
	if (status != STATUS_OK)
	{
		MAIN_DBG(BOLD(BRIGHT_RED("Error (status: %u). Aborting\n")), status);
		return 0;
	}

	status = LEX_init();

	if (status != STATUS_OK)
	{
		MAIN_DBG(BOLD(BRIGHT_RED("Error (status: %u). Aborting\n")), status);
		return 0;
	}

	LEX_run_fsm();

	PARSE_init();
	PARSE_run_rdp();
}