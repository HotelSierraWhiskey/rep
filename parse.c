#include "parse.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#ifdef DEBUG_PARSE
#define PARSE_DBG(fmt, ...) printf("PARSE:\t" fmt, ##__VA_ARGS__)
#else
#define PARSE_DBG(fmt, ...) asm("nop");
#endif

/****************************************************************************************************
 *	T Y P E D E F S
 ****************************************************************************************************/

typedef struct
{
	PARSE_node_t *				p_root;
	const LEX_token_list_t *	p_token_list;
	uint32_t 					u32_current_token_index;
	uint32_t					u32_num_statements;
} PARSE_info_t;

typedef enum
{
	PARSE_RULE_EXPRESSION = 0,
	PARSE_RULE_FACTOR,
	PARSE_RULE_TERM,
	////////////////////
	PARSE_RULE_NUM_RULES
} PARSE_rule_t;

typedef enum
{
	PARSE_NODE_SIDE_ROOT = 0,
	PARSE_NODE_SIDE_LEFT,
	PARSE_NODE_SIDE_RIGHT,
	////////////////////
	PARSE_NODE_SIDE_NUM_SIDES
} PARSE_node_side_t;

/****************************************************************************************************
 *	S T A T I C   V A R I A B L E S
 ****************************************************************************************************/

/*
 *	Parser info struct
 */
static PARSE_info_t parse_info;

/****************************************************************************************************
 *	S T A T I C   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

/*
 *	Parsing rules
 */
static PARSE_node_t * 				PARSE_expression		(void);
static PARSE_node_t * 				PARSE_term				(void);
static PARSE_node_t * 				PARSE_factor			(void);

/*
 *	Helpers
 */
static void 						PARSE_traverse_tree		(const PARSE_node_t *p_node, uint32_t u32_level, uint8_t side);
static void 						PARSE_consume_token		(void);
static inline LEX_token_t * 		PARSE_get_current_token	(void);
static inline PARSE_node_t *		PARSE_create_node		(LEX_token_t * p_token, PARSE_node_t * p_left, PARSE_node_t * p_right);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/*
 *	Initializes the module
 */
void PARSE_init(void)
{
	PARSE_DBG("Initializing\n");

	parse_info.p_token_list = LEX_get_token_list();
	parse_info.u32_num_statements = LEX_get_num_statements();
	parse_info.u32_current_token_index = 0;
}

/*
 *	Runs the recursive descent parser
 */
void PARSE_run_rdp(void)
{
	PARSE_node_t *p_root;

	for (uint32_t i = 0; i < LEX_get_num_statements(); i++)
	{
		p_root = PARSE_expression();
		PARSE_DBG("Tree:\n");
		PARSE_traverse_tree(p_root, 0, PARSE_NODE_SIDE_ROOT);
	}
}

/****************************************************************************************************
 *	S T A T I C   F U N C T I O N S
 ****************************************************************************************************/

/*
 *	Moves the current token index forward
 */
static void PARSE_consume_token(void)
{
	parse_info.u32_current_token_index++;
}

/*
 *	For visual debugging only
 */
static void PARSE_traverse_tree(const PARSE_node_t *p_node, uint32_t u32_level, uint8_t u8_side)
{
	const char kpc_sides[PARSE_NODE_SIDE_NUM_SIDES] =
	{
		[PARSE_NODE_SIDE_ROOT]	= 'X',
		[PARSE_NODE_SIDE_LEFT] 	= 'L',
		[PARSE_NODE_SIDE_RIGHT]	= 'R',
	};

	if (p_node == NULL)
	{
		return;
	}

	for (uint8_t i = 0; i < u32_level; i++)
	{
		if (i == u32_level - 1)
		{
			printf("|%c -> ", kpc_sides[u8_side]);
		}
		else
		{
			printf("      ");
		}
	}

	printf("%s\n", p_node->p_token.pc_lexeme);

	PARSE_traverse_tree(p_node->p_left, u32_level + 1, PARSE_NODE_SIDE_LEFT);
	PARSE_traverse_tree(p_node->p_right, u32_level + 1, PARSE_NODE_SIDE_RIGHT);
}

/*
 *	Retrieves the token at the current token index
 */
static inline LEX_token_t * PARSE_get_current_token(void)
{
	return &parse_info.p_token_list->p_tokens[parse_info.u32_current_token_index];
}

/*
 *	Creates a node for use in the current parse tree
 */
static inline PARSE_node_t * PARSE_create_node (LEX_token_t * p_token, PARSE_node_t * p_left, PARSE_node_t * p_right)
{
	PARSE_node_t * p_node = (PARSE_node_t *)malloc(sizeof(PARSE_node_t));
	strncpy(p_node->p_token.pc_lexeme, p_token->pc_lexeme, LEX_MAX_LEXEME_SIZE);
	p_node->p_token.u32_row = p_token->u32_row;
	p_node->p_token.u32_column = p_token->u32_column;
	p_node->p_left = p_left;
	p_node->p_right = p_right;
	return p_node;
}

/****************************************************************************************************
 *	G R A M M A R   R U L E S
 ****************************************************************************************************/

/*
 *	Factor grammar rule
 */
static PARSE_node_t * PARSE_factor(void)
{
	PARSE_node_t * 	p_node;
	LEX_token_t *	p_token = PARSE_get_current_token();
	LEX_token_t		saved_token;
	
	PARSE_DBG("[%s] FACTOR\n", p_token->pc_lexeme);

	switch (p_token->type)
	{
		case LEX_TOKEN_TYPE_OPEN_PAREN:
		{
			PARSE_consume_token();
			p_node = PARSE_expression();
			PARSE_consume_token();
			return p_node;
		}
		case LEX_TOKEN_TYPE_INT_LITERAL:
		{
			memcpy(&saved_token, p_token, sizeof(LEX_token_t));
			PARSE_consume_token();
			return PARSE_create_node(&saved_token, NULL, NULL);
		}
	}

	return NULL; // Unreached
}

/*
 *	Term grammar rule
 */
static PARSE_node_t * PARSE_term(void)
{
	PARSE_node_t * 	p_node = PARSE_factor();
	LEX_token_t *	p_token = PARSE_get_current_token();
	LEX_token_t		saved_token;

	PARSE_DBG("[%s] TERM\n", p_token->pc_lexeme);

	switch (p_token->type)
	{
		case LEX_TOKEN_TYPE_OP_MULTIPLY:
		case LEX_TOKEN_TYPE_OP_DIVIDE:
		case LEX_TOKEN_TYPE_OP_ADD:
		case LEX_TOKEN_TYPE_OP_SUBTRACT:
		{
			memcpy(&saved_token, p_token, sizeof(LEX_token_t));
			PARSE_consume_token();
			p_node = PARSE_create_node(&saved_token, p_node, PARSE_factor());
		}
	}

	return p_node;
}

/*
 *	Expression grammar rule
 */
static PARSE_node_t * PARSE_expression(void)
{
	PARSE_node_t * 	p_node = PARSE_term();
	LEX_token_t *	p_token = PARSE_get_current_token();
	LEX_token_t		saved_token;

	PARSE_DBG("[%s] EXPRESSION\n", p_token->pc_lexeme);

	switch (p_token->type)
	{
		case LEX_TOKEN_TYPE_OP_MULTIPLY:
		case LEX_TOKEN_TYPE_OP_DIVIDE:
		case LEX_TOKEN_TYPE_OP_ADD:
		case LEX_TOKEN_TYPE_OP_SUBTRACT:
		{
			memcpy(&saved_token, p_token, sizeof(LEX_token_t));
			PARSE_consume_token();
			p_node = PARSE_create_node(&saved_token, p_node, PARSE_term());
		}
		case LEX_TOKEN_TYPE_DELIM:
		{
			PARSE_consume_token();
		}
	}

	return p_node;
}
