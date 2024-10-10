#ifndef LEX_H
#define LEX_H

#include "common.h"
#include "status.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#define LEX_MAX_LEXEME_SIZE				(0xFF)
#define LEX_MAX_LEXEME_SIZE_EXCEEDED	"MAX_LEXEME_SIZE_EXCEEDED"

/****************************************************************************************************
 *	T Y P E D E F S
 ****************************************************************************************************/

typedef enum
{
	LEX_TOKEN_TYPE_UNKNOWN = 0,
	LEX_TOKEN_TYPE_IDENTIFIER,
	LEX_TOKEN_TYPE_INT_LITERAL,
	LEX_TOKEN_TYPE_OPEN_PAREN,
	LEX_TOKEN_TYPE_CLOSE_PAREN,
	LEX_TOKEN_TYPE_OP_ASSIGNMENT,
	LEX_TOKEN_TYPE_OP_ADD,
	LEX_TOKEN_TYPE_OP_SUBTRACT,
	LEX_TOKEN_TYPE_OP_MULTIPLY,
	LEX_TOKEN_TYPE_OP_DIVIDE,
	LEX_TOKEN_TYPE_DELIM,
	//////////////////////////////
	LEX_TOKEN_TYPE_NUM_TYPES
} LEX_token_type_t;

typedef struct _LEX_token
{
	LEX_token_type_t 	type;
	char 				pc_lexeme[LEX_MAX_LEXEME_SIZE];
	uint32_t			u32_row;
	uint32_t			u32_column;
} LEX_token_t;

typedef struct _LEX_token_list
{
	LEX_token_t *		p_tokens;
	uint32_t			u32_num_tokens;
} LEX_token_list_t;

/****************************************************************************************************
 *	P U B L I C   F U N C T I O N S
 ****************************************************************************************************/

STATUS_t 					LEX_init						(void);
STATUS_t 					LEX_deinit						(void);
void 						LEX_run_fsm						(void);
const LEX_token_list_t *	LEX_get_token_list				(void);
const uint32_t 				LEX_get_num_statements 			(void);
const char * 				LEX_get_token_type_descriptor 	(const LEX_token_type_t k_token_type);

#endif
