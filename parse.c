#include "parse.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#ifdef DEBUG_PARSE
#define PARSE_DBG(fmt, ...)				printf(BOLD("PARSE:\t")fmt, ##__VA_ARGS__)
#define PARSE_GREEN(fmt, ...)			printf(BOLD(BRIGHT_GREEN("PARSE:\t"))fmt, ##__VA_ARGS__)
#define PARSE_WARN(fmt, ...)			printf(BOLD(BRIGHT_YELLOW("PARSE:\t"))fmt, ##__VA_ARGS__)
#define PARSE_ERR(fmt, ...)				printf(BOLD(BRIGHT_RED("PARSE:\t"))fmt, ##__VA_ARGS__)
#else
#define PARSE_DBG(fmt, ...)
#define PARSE_GREEN(fmt, ...)
#define PARSE_WARN(fmt, ...)
#define PARSE_ERR(fmt, ...)
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
	uint32_t					u32_num_trees;
	PARSE_tree_container_t		trees;
} PARSE_info_t;

typedef enum
{
	PARSE_RULE_EXPRESSION = 0,
	PARSE_RULE_FACTOR,
	PARSE_RULE_TERM,
	////////////////////
	PARSE_RULE_NUM_RULES
} PARSE_rule_t;

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
static PARSE_node_t * 				PARSE_statement			(void);
static PARSE_node_t * 				PARSE_expression		(void);
static PARSE_node_t * 				PARSE_term				(void);
static PARSE_node_t * 				PARSE_factor			(void);

/*
 *	Helpers
 */
static void 						PARSE_consume_token		(void);
static inline LEX_token_t * 		PARSE_get_current_token	(void);
static inline LEX_token_t * 		PARSE_get_next_token	(void);
static inline PARSE_node_t *		PARSE_create_node		(LEX_token_t * p_token, PARSE_node_t * p_left, PARSE_node_t * p_right);
static void 						PARSE_save_tree			(const PARSE_node_t * p_root);

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
	parse_info.u32_num_trees = 0;
	parse_info.trees = malloc(sizeof(PARSE_node_t *));
}

/*
 *	Runs the recursive descent parser
 */
void PARSE_run_rdp(void)
{
	PARSE_node_t *p_root;

	for (uint32_t i = 0; i < LEX_get_num_statements(); i++)
	{
		p_root = PARSE_statement();
		PARSE_save_tree(p_root);
		PARSE_DBG("Tree:\n");
		PARSE_traverse_tree(p_root, 0, PARSE_NODE_SIDE_ROOT);
		PARSE_consume_token();
	}

	PARSE_DBG(BOLD(BRIGHT_GREEN("Done\n")));
}

/*
 *	Get all parse trees
 */
const PARSE_tree_container_t PARSE_get_all_trees (void)
{
	return parse_info.trees;
}

/*
 *	Get the number of trees parsed
 */
uint32_t PARSE_get_num_trees(void)
{
	return parse_info.u32_num_trees;
}

void PARSE_traverse_tree(const PARSE_node_t *p_node, uint32_t u32_level, uint8_t u8_side)
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
 *	Retrieves the token at the current token index
 */
static inline LEX_token_t * PARSE_get_current_token(void)
{
	return &parse_info.p_token_list->p_tokens[parse_info.u32_current_token_index];
}

static inline LEX_token_t * PARSE_get_next_token(void)
{
	if (parse_info.p_token_list->u32_num_tokens == parse_info.u32_current_token_index)
	{
		return NULL;
	}
	return &parse_info.p_token_list->p_tokens[parse_info.u32_current_token_index + 1];
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

static void PARSE_save_tree(const PARSE_node_t * p_root)
{
	ASSERT(p_root);
	parse_info.trees = realloc(parse_info.trees, sizeof(PARSE_node_t *) * parse_info.u32_num_trees + 1);
	parse_info.trees[parse_info.u32_num_trees++] = p_root;
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
			p_node = PARSE_statement();
			PARSE_consume_token();
			return p_node;
		}
		case LEX_TOKEN_TYPE_INT_LITERAL:
		{
			memcpy(&saved_token, p_token, sizeof(LEX_token_t));
			PARSE_consume_token();
			return PARSE_create_node(&saved_token, NULL, NULL);
		}
		case LEX_TOKEN_TYPE_IDENTIFIER:
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

	while (p_token->type == LEX_TOKEN_TYPE_OP_MULTIPLY || p_token->type == LEX_TOKEN_TYPE_OP_DIVIDE)
	{
		memcpy(&saved_token, p_token, sizeof(LEX_token_t));
		PARSE_consume_token();

		if (p_token->type == LEX_TOKEN_TYPE_OP_DIVIDE)
		{
			p_node = PARSE_create_node(&saved_token, p_node, PARSE_factor());
		}
		else if (p_token->type == LEX_TOKEN_TYPE_OP_MULTIPLY)
		{
			p_node = PARSE_create_node(&saved_token, p_node, PARSE_term());
		}
		
		p_token = PARSE_get_current_token();
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

	while (p_token->type == LEX_TOKEN_TYPE_OP_SUBTRACT || p_token->type == LEX_TOKEN_TYPE_OP_ADD)
	{
		memcpy(&saved_token, p_token, sizeof(LEX_token_t));
		PARSE_consume_token();

		if (p_token->type == LEX_TOKEN_TYPE_OP_ADD)
		{
			p_node = PARSE_create_node(&saved_token, p_node, PARSE_expression());
		}
		else if (p_token->type == LEX_TOKEN_TYPE_OP_SUBTRACT)
		{
			p_node = PARSE_create_node(&saved_token, p_node, PARSE_term());
		}

		p_token = PARSE_get_current_token();
	}

	return p_node;
}

/*
 *	Statement grammar rule
 */
static PARSE_node_t * PARSE_statement(void)
{
	PARSE_node_t * 	p_node = PARSE_expression();
	LEX_token_t *	p_token = PARSE_get_current_token();
	LEX_token_t		saved_token;

	PARSE_DBG("[%s] STATEMENT\n", p_token->pc_lexeme);

	while (	p_token->type == LEX_TOKEN_TYPE_OP_ASSIGNMENT)
	{
		memcpy(&saved_token, p_token, sizeof(LEX_token_t));
		PARSE_consume_token();
		p_node = PARSE_create_node(&saved_token, p_node, PARSE_expression());

		if (PARSE_get_current_token()->type == LEX_TOKEN_TYPE_DELIM)
		{
			break;
		}
	}

	return p_node;
}
