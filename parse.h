#ifndef PARSE_H
#define PARSE_H

#include "lex.h"

typedef struct _PARSE_node
{
	LEX_token_t				p_token;
	struct _PARSE_node *	p_left;
	struct _PARSE_node *	p_right;
} PARSE_node_t;

void 	PARSE_init		(void);
void 	PARSE_run_rdp	(void);

#endif