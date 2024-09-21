#include "common.h"
#include "io_handler.h"
#include "lex.h"
#include "parse.h"

int main(int argc, char** argv)
{
#ifdef BUILD_DEBUG
	IO_HANDLER_load_source_file("debug.rep");
#else
	if (argc != 2)
	{
		return 0;
	}

	const char * fname = argv[1];

	IO_HANDLER_load_source_file(fname);
#endif // BUILD_DEBUG

	LEX_init();
	LEX_run_fsm();

	PARSE_init();
	PARSE_run_rdp();
}