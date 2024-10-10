#include "code_gen.h"
#include "io_handler.h"
#include "lex.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#ifdef DEBUG_CODE_GEN
#define CODE_GEN_DBG(fmt, ...)			printf(BOLD("CODE_GEN:\t")fmt, ##__VA_ARGS__)
#define CODE_GEN_GREEN(fmt, ...)		printf(BOLD(BRIGHT_GREEN("CODE_GEN:\t"))fmt, ##__VA_ARGS__)
#define CODE_GEN_WARN(fmt, ...)			printf(BOLD(BRIGHT_YELLOW("CODE_GEN:\t"))fmt, ##__VA_ARGS__)
#define CODE_GEN_ERR(fmt, ...)			printf(BOLD(BRIGHT_RED("CODE_GEN:\t"))fmt, ##__VA_ARGS__)
#else
#define CODE_GEN_DBG(fmt, ...)
#define CODE_GEN_GREEN(fmt, ...)
#define CODE_GEN_WARN(fmt, ...)
#define CODE_GEN_ERR(fmt, ...)
#endif

#define INSTRUCTION_FMT(instruction)	BRIGHT_YELLOW(instruction)

/****************************************************************************************************
 *	T Y P E D E F S
 ****************************************************************************************************/

typedef struct
{
	uint32_t	u32_label_index;
} CODE_GEN_info_t;

/****************************************************************************************************
 *	S T A T I C   V A R I A B L E S
 ****************************************************************************************************/

// None

/****************************************************************************************************
 *	S T A T I C   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

/*
 *	Code generation handlers
 */
static void 					CODE_GEN_handle_node						(PARSE_node_t * p_node);
static void 					CODE_GEN_handle_EXPR_TYPE_ID				(PARSE_node_t * p_node);
static void 					CODE_GEN_handle_EXPR_TYPE_ADD				(PARSE_node_t * p_node);
static void 					CODE_GEN_handle_EXPR_TYPE_SUBTRACT			(PARSE_node_t * p_node);
static void 					CODE_GEN_handle_EXPR_TYPE_MULTIPLY			(PARSE_node_t * p_node);
static void 					CODE_GEN_handle_EXPR_TYPE_DIVIDE			(PARSE_node_t * p_node);
static void 					CODE_GEN_handle_EXPR_TYPE_ASSIGNMENT		(PARSE_node_t * p_node);
static void 					CODE_GEN_handle_STATEMENT_TYPE_ASSIGNMENT	(PARSE_node_t * p_node);

/*
 *	Helpers
 */
void 							CODE_GEN_create_label						(void);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void CODE_GEN_traverse_tree(PARSE_node_t *p_root)
{
    if (!p_root)
	{
		return;
	}

	// Handle leaves first
    if (!p_root->p_left && !p_root->p_right)
	{
		CODE_GEN_handle_node(p_root);
    }
	else
	{
		if (p_root->p_left)
		{
			CODE_GEN_traverse_tree(p_root->p_left);
		}

		if (p_root->p_right)
		{
			CODE_GEN_traverse_tree(p_root->p_right);
		}

		CODE_GEN_handle_node(p_root);
	}
}

static void CODE_GEN_handle_node(PARSE_node_t * p_node)
{
	if (p_node == NULL)
	{
		return;
	}

	switch(p_node->type)
	{
		case PARSE_NODE_TYPE_ID:
		{
			CODE_GEN_handle_EXPR_TYPE_ID(p_node);
			break;
		}
		case PARSE_NODE_TYPE_EXPR_TYPE_ADD:
		{
			CODE_GEN_handle_EXPR_TYPE_ADD(p_node);
			break;
		}
		case PARSE_NODE_TYPE_EXPR_TYPE_SUBTRACT:
		{
			CODE_GEN_handle_EXPR_TYPE_SUBTRACT(p_node);
			break;
		}
		case PARSE_NODE_TYPE_EXPR_TYPE_MULTIPLY:
		{
			CODE_GEN_handle_EXPR_TYPE_MULTIPLY(p_node);
			break;
		}
		case PARSE_NODE_TYPE_EXPR_TYPE_DIVIDE:
		{
			CODE_GEN_handle_EXPR_TYPE_DIVIDE(p_node);
			break;
		}
		case PARSE_NODE_TYPE_EXPR_TYPE_ASSIGNMENT:
		{
			CODE_GEN_handle_EXPR_TYPE_ASSIGNMENT(p_node);
			break;
		}
		case PARSE_NODE_TYPE_STATEMENT_TYPE_ASSIGNMENT:
		{
			CODE_GEN_handle_STATEMENT_TYPE_ASSIGNMENT(p_node);
			break;
		}
		default:
		{
			ASSERT(0);
		}
	}
}

static void CODE_GEN_handle_EXPR_TYPE_ID(PARSE_node_t * p_node)
{
	p_node->scratch_register = SCRATCH_REGISTER_alloc();
	CODE_GEN_DBG(INSTRUCTION_FMT("mov") " %s, %s\n", p_node->p_token->pc_lexeme, 
		SCRATCH_REGISTER_get_register_name(p_node->scratch_register));
}

static void CODE_GEN_handle_EXPR_TYPE_ADD(PARSE_node_t * p_node)
{
	CODE_GEN_DBG(INSTRUCTION_FMT("add") " %s, %s\n", 
		SCRATCH_REGISTER_get_register_name(p_node->p_left->scratch_register), 
		SCRATCH_REGISTER_get_register_name(p_node->p_right->scratch_register));

	p_node->scratch_register = p_node->p_right->scratch_register;
	SCRATCH_REGISTER_free(p_node->p_left->scratch_register);
}

static void CODE_GEN_handle_EXPR_TYPE_SUBTRACT(PARSE_node_t * p_node)
{
    CODE_GEN_DBG(INSTRUCTION_FMT("sub") " %s, %s\n", 
        SCRATCH_REGISTER_get_register_name(p_node->p_left->scratch_register), 
        SCRATCH_REGISTER_get_register_name(p_node->p_right->scratch_register));
    
    p_node->scratch_register = p_node->p_right->scratch_register;  
    SCRATCH_REGISTER_free(p_node->p_left->scratch_register);
}

static void CODE_GEN_handle_EXPR_TYPE_MULTIPLY(PARSE_node_t * p_node)
{
    CODE_GEN_DBG(INSTRUCTION_FMT("mul") " %s, %s\n", 
        SCRATCH_REGISTER_get_register_name(p_node->p_left->scratch_register), 
        SCRATCH_REGISTER_get_register_name(p_node->p_right->scratch_register));
    
    p_node->scratch_register = p_node->p_right->scratch_register;  
    SCRATCH_REGISTER_free(p_node->p_left->scratch_register);
}

static void CODE_GEN_handle_EXPR_TYPE_DIVIDE(PARSE_node_t * p_node)
{
    CODE_GEN_DBG(INSTRUCTION_FMT("div") " %s, %s\n", 
        SCRATCH_REGISTER_get_register_name(p_node->p_left->scratch_register), 
        SCRATCH_REGISTER_get_register_name(p_node->p_right->scratch_register));
    
    p_node->scratch_register = p_node->p_right->scratch_register;  
    SCRATCH_REGISTER_free(p_node->p_left->scratch_register);
}

static void CODE_GEN_handle_EXPR_TYPE_ASSIGNMENT(PARSE_node_t * p_node)
{

}

static void CODE_GEN_handle_STATEMENT_TYPE_ASSIGNMENT(PARSE_node_t * p_node)
{
	p_node->scratch_register = SCRATCH_REGISTER_alloc();
    CODE_GEN_DBG(INSTRUCTION_FMT("mov") " %s, %s\n", 
        SCRATCH_REGISTER_get_register_name(p_node->p_right->scratch_register), 
        SCRATCH_REGISTER_get_register_name(p_node->p_left->scratch_register));
    
    SCRATCH_REGISTER_free(p_node->p_right->scratch_register);
}
