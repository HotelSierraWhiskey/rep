#include "common.h"
#include "status.h"
#include "io_handler.h"
#include "lex.h"
#include "parse.h"

int main(int argc, char** argv)
{
	STATUS_t status;
#ifdef BUILD_DEBUG
	status = IO_HANDLER_load_source_file("debug.rep");
#else
	if (argc != 2)
	{
		return 0;
	}

	const char * fname = argv[1];

	status = IO_HANDLER_load_source_file(fname);
#endif // BUILD_DEBUG
	if (status != STATUS_OK)
	{
		return 0;
	}

	LEX_init();
	LEX_run_fsm();

	PARSE_init();
	PARSE_run_rdp();
}