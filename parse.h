#ifndef PARSE_H
#define PARSE_H

#include "lex.h"

/****************************************************************************************************
 *	T Y P E D E F S
 ****************************************************************************************************/

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
	LEX_token_t				p_token;
	struct _PARSE_node *	p_left;
	struct _PARSE_node *	p_right;
} PARSE_node_t;

typedef const PARSE_node_t ** PARSE_tree_array_t;

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
const PARSE_tree_list_t *		PARSE_get_tree_list		(void);
void 							PARSE_traverse_tree		(const PARSE_node_t *p_node, uint32_t u32_level, uint8_t side);

#endif