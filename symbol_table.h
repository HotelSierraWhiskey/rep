#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "lex.h"
#include "builtins.h"

typedef struct _SYMBOL_TABLE_entry
{
	const LEX_token_t * 		p_token;
	const BUILTINS_type_t 		builtin_type;
	const uint32_t				u32_scope_id;
} SYMBOL_TABLE_entry_t;

const SYMBOL_TABLE_entry_t * 	SYMBOL_TABLE_get_symbol_table	(void);
STATUS_t 						SYMBOL_TABLE_append_symbol		(const SYMBOL_TABLE_entry_t * kp_symbol);

#endif