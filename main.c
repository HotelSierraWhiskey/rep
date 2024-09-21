#include "common.h"
#include "io_handler.h"
#include "lex.h"
#include "parse.h"

int main()
{

	IO_HANDLER_load_source_file("test.rep");

	LEX_init();
	LEX_run_fsm();

	PARSE_init();
	PARSE_run_rdp();

	

	

	// for (uint32_t i = 0; i < p_token_list->u32_num_tokens; i++)
	// {
	// 	LEX_token_t token = p_token_list->p_tokens[i];

	// 	printf("lexeme: %-20s\ttype: %-20s\trow: %-5u\tcolumn: %-5u\n", 
	// 		token.pc_lexeme, CFG_get_terminal_descriptor(token.type), token.u32_row, token.u32_column);
	// }
}