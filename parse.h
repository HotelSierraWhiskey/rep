#ifndef PARSE_H
#define PARSE_H

#include "lex.h"
#include "scratch_register.h"

/****************************************************************************************************
 *	T Y P E D E F S
 ****************************************************************************************************/

typedef enum
{
	PARSE_NODE_TYPE_ID,
	PARSE_NODE_TYPE_EXPR_TYPE_ADD,
	PARSE_NODE_TYPE_EXPR_TYPE_SUBTRACT,
	PARSE_NODE_TYPE_EXPR_TYPE_MULTIPLY,
	PARSE_NODE_TYPE_EXPR_TYPE_DIVIDE,
	PARSE_NODE_TYPE_EXPR_TYPE_ASSIGNMENT,
	PARSE_NODE_TYPE_STATEMENT_TYPE_ASSIGNMENT,
	//////////////////////////////
	PARSE_NODE_TYPE_NUM_TYPES,
} PARSE_node_type_t;

typedef enum
{
	PARSE_NODE_SIDE_ROOT = 0,
	PARSE_NODE_SIDE_LEFT,
	PARSE_NODE_SIDE_RIGHT,
	////////////////////
	PARSE_NODE_SIDE_NUM_SIDES
} PARSE_node_side_t;

typedef struct _PARSE_node
{
	PARSE_node_type_t		type;
	LEX_token_t *			p_token;
	SCRATCH_REGISTER_id_t	scratch_register;
	struct _PARSE_node *	p_left;
	struct _PARSE_node *	p_right;
} PARSE_node_t;

typedef PARSE_node_t ** PARSE_tree_array_t;

typedef struct _PARSE_tree_list
{
	PARSE_tree_array_t 	trees;
	uint32_t			u32_num_trees;
} PARSE_tree_list_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void 							PARSE_init				(void);
void 							PARSE_run_rdp			(void);
PARSE_tree_list_t *				PARSE_get_tree_list		(void);
void 							PARSE_traverse_tree		(const PARSE_node_t *p_node, uint32_t u32_level, uint8_t side);

#endif