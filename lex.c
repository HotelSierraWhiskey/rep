#include "io_handler.h"
#include "lex.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#ifdef DEBUG_LEX
#define LEX_DBG(fmt, ...)				printf("LEX:\t"fmt, ##__VA_ARGS__)
#else
#define LEX_DBG(fmt, ...)
#endif

#define LEX_INITIAL_TOKEN_BUFFER_SIZE	(4)

/*
 *	Handy macros to group character subsets
 */
#define LEX_SCANNING_ALPHA(c)			(isalpha((char)c)) || c == '_'
#define LEX_SCANNING_NUMBER(c)			(isdigit((char)c))
#define LEX_SCANNING_OPERATOR(c)		(c == '+' || c == '-' || c == '*' || c == '/' || c == '=')
#define LEX_SCANNING_CONTROL_CHAR(c)	(c == '(' || c == ')')
#define LEX_SCANNING_NEWLINE(c)			(c == '\n')
#define LEX_SCANNING_WHITESPACE(c)		(c == ' ' || c == '\t' || LEX_SCANNING_NEWLINE(c) || c == '\r' || c == '\v' || c == '\f')
#define LEX_SCANNING_DELIM(c)			(c == ';')

/****************************************************************************************************
 *	T Y P E D E F S
 ****************************************************************************************************/

typedef bool (* LEX_fsm_state_handler_t) (void);

typedef enum
{
	LEX_FSM_STATE_ID_START,
	LEX_FSM_STATE_ID_WAIT_SCANNING_WHITESPACE,
	LEX_FSM_STATE_ID_WAIT_SCANNING_DELIM,
	LEX_FSM_STATE_ID_WAIT_SCANNING_IDENTIFIER,
	LEX_FSM_STATE_ID_WAIT_SCANNING_NUMBER,
	LEX_FSM_STATE_ID_WAIT_SCANNING_OPERATOR,
	LEX_FSM_STATE_ID_WAIT_SCANNING_CONTROL_CHAR,
	//////////////////////////////
	LEX_FSM_STATE_ID_NUM_STATES
} LEX_fsm_state_id_t;

typedef struct _LEX_fsm_state
{
	LEX_fsm_state_id_t			id;
	LEX_fsm_state_handler_t 	handler;
	const char *				descriptor;
} LEX_fsm_state_t;

typedef struct _LEX_fsm_info
{
	LEX_fsm_state_t *		p_state;
	char					c_current_char;
	uint32_t				u32_row;
	uint32_t				u32_column;
	LEX_token_t				current_token;
	LEX_token_list_t		token_list;
	uint32_t				u32_token_buffer_capacity;
	char					p_current_lexeme[LEX_MAX_LEXEME_SIZE];
	uint8_t					u8_current_lexeme_index;
	uint32_t				u32_num_statements;
} LEX_info_t;

/****************************************************************************************************
 *	S T A T I C   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

/*
 *	FSM-related functions
 */
static bool 				LEX_handle_STATE_START							(void);
static bool 				LEX_handle_STATE_WAIT_SCANNING_IDENTIFIER			(void);
static bool 				LEX_handle_STATE_WAIT_SCANNING_NUMBER			(void);
static bool 				LEX_handle_STATE_WAIT_SCANNING_OPERATOR			(void);
static bool 				LEX_handle_STATE_WAIT_SCANNING_CONTROL_CHAR		(void);
static inline void 			LEX_go_to_state									(LEX_fsm_state_id_t state_id);

/*
 *	Functions concerned with token construction
 */
static inline void 			LEX_push_to_current_lexeme						(char c);
static void 				LEX_flush_to_token								(void);
static LEX_token_type_t		LEX_token_type_from_lexeme						(void);

/*
 *	Debug & helpers
 */
static const char * 		LEX_get_token_type_descriptor					(const LEX_token_type_t k_token_type);
static void 				LEX_fsm_report 									(void);
static void					LEX_restore_defaults							(void);


/****************************************************************************************************
 *	S T A T I C   V A R I A B L E S
 ****************************************************************************************************/

static const char * const pk_token_type_descriptors[LEX_TOKEN_TYPE_NUM_TYPES] =
{
	[LEX_TOKEN_TYPE_UNKNOWN] 		= "UNKNOWN",
	[LEX_TOKEN_TYPE_IDENTIFIER] 	= "IDENTIFIER",
	[LEX_TOKEN_TYPE_INT_LITERAL] 	= "INT_LITERAL",
	[LEX_TOKEN_TYPE_OPEN_PAREN] 	= "OPEN_PAREN",
	[LEX_TOKEN_TYPE_CLOSE_PAREN] 	= "CLOSE_PAREN",
	[LEX_TOKEN_TYPE_OP_ASSIGNMENT] 	= "OP_ASSIGNMENT",
	[LEX_TOKEN_TYPE_OP_ADD] 		= "OP_ADD",
	[LEX_TOKEN_TYPE_OP_SUBTRACT] 	= "OP_SUBTRACT",
	[LEX_TOKEN_TYPE_OP_MULTIPLY] 	= "OP_MULTIPLY",
	[LEX_TOKEN_TYPE_OP_DIVIDE] 		= "OP_DIVIDE",
	[LEX_TOKEN_TYPE_DELIM] 			= "DELIM",
};

/*
 *	This table contains all LEX FSM states
 */
static LEX_fsm_state_t p_fsm_states[LEX_FSM_STATE_ID_NUM_STATES] =
{
	[LEX_FSM_STATE_ID_START] = 
	{
		.id 		= LEX_FSM_STATE_ID_START,
		.handler 	= LEX_handle_STATE_START,
		.descriptor = "STATE_START"
	},
	[LEX_FSM_STATE_ID_WAIT_SCANNING_WHITESPACE] = 
	{
		.id 		= LEX_FSM_STATE_ID_WAIT_SCANNING_WHITESPACE,
		.handler 	= LEX_handle_STATE_START,
		.descriptor = "STATE_WAIT_SCANNING_WHITESPACE"
	},
	[LEX_FSM_STATE_ID_WAIT_SCANNING_DELIM] = 
	{
		.id 		= LEX_FSM_STATE_ID_WAIT_SCANNING_DELIM,
		.handler 	= LEX_handle_STATE_START,
		.descriptor = "STATE_ID_WAIT_SCANNING_DELIM"
	},
	[LEX_FSM_STATE_ID_WAIT_SCANNING_IDENTIFIER] = 
	{
		.id 		= LEX_FSM_STATE_ID_WAIT_SCANNING_IDENTIFIER,
		.handler 	= LEX_handle_STATE_WAIT_SCANNING_IDENTIFIER,
		.descriptor = "STATE_WAIT_SCANNING_IDENTIFIER"
	},
	[LEX_FSM_STATE_ID_WAIT_SCANNING_NUMBER] = 
	{
		.id 		= LEX_FSM_STATE_ID_WAIT_SCANNING_NUMBER,
		.handler 	= LEX_handle_STATE_WAIT_SCANNING_NUMBER,
		.descriptor = "STATE_WAIT_SCANNING_NUMBER"
	},
	[LEX_FSM_STATE_ID_WAIT_SCANNING_OPERATOR] = 
	{
		.id 		= LEX_FSM_STATE_ID_WAIT_SCANNING_OPERATOR,
		.handler 	= LEX_handle_STATE_WAIT_SCANNING_OPERATOR,
		.descriptor = "STATE_WAIT_SCANNING_OPERATOR"
	},
	[LEX_FSM_STATE_ID_WAIT_SCANNING_CONTROL_CHAR] = 
	{
		.id 		= LEX_FSM_STATE_ID_WAIT_SCANNING_CONTROL_CHAR,
		.handler 	= LEX_handle_STATE_WAIT_SCANNING_CONTROL_CHAR,
		.descriptor = "STATE_WAIT_SCANNING_CONTROL_CHAR"
	},
};

/*
 *	The LEX FSM info struct
 */
static LEX_info_t lex_info;

/****************************************************************************************************
 *	P U B L I C   F U N C T I O N   D E F I N I T I O N S
 ****************************************************************************************************/

/*
 *	Initializes the module
 *
 *	For the moment this just returns STATUS_OK
 */
STATUS_t LEX_init(void)
{
	LEX_DBG("Initializing\n");

	LEX_restore_defaults();
	lex_info.token_list.p_tokens = (LEX_token_t *)malloc(sizeof(LEX_token_t) * LEX_INITIAL_TOKEN_BUFFER_SIZE);

	return STATUS_OK;
}

/*
 *	Deinitializes the module
 *
 *	For the moment this just returns STATUS_OK
 */
STATUS_t LEX_deinit(void)
{
	LEX_DBG("Deinitializing\n");

	LEX_restore_defaults();
	free(lex_info.token_list.p_tokens);
	lex_info.token_list.p_tokens = NULL;

	return STATUS_OK;
}

/*
 *	The top-level LEX FSM runner
 */
void LEX_run_fsm(void)
{
	const IO_HANDLER_source_info_t * kp_source_info = IO_HANDLER_get_source_info();
	const char * kpc_source_ptr = kp_source_info->pc_source_buffer;
	const char * kpc_source_end = kp_source_info->pc_source_buffer + kp_source_info->u32_size;

	LEX_DBG("FSM Transitions:\n");

	while (kpc_source_ptr < kpc_source_end)
	{
		lex_info.c_current_char = *kpc_source_ptr++;

		LEX_fsm_report();

		ASSERT(lex_info.p_state->handler());
	}

	// Flush the last buffer
	LEX_flush_to_token();

	LEX_DBG("Found %u statements\n", lex_info.u32_num_statements);
	LEX_DBG("Produced %u tokens:\n", lex_info.token_list.u32_num_tokens);

	for (uint32_t i = 0; i < lex_info.token_list.u32_num_tokens; i++)
	{
		LEX_DBG("Lexeme: %-30s(%s)\n", 
					lex_info.token_list.p_tokens[i].pc_lexeme, 
					LEX_get_token_type_descriptor(lex_info.token_list.p_tokens[i].type));
	}
}

/*
 *	Retrieves the token list
 */
const LEX_token_list_t * LEX_get_token_list(void)
{
	return &lex_info.token_list;
}

/*
 *	Retrieves the statement count
 */
const uint32_t LEX_get_num_statements (void)
{
	return lex_info.u32_num_statements;
}

/****************************************************************************************************
 *	S T A T I C   F U N C T I O N   D E F I N I T I O N S
 ****************************************************************************************************/

/*
 *	For use by handlers to change `p_fsm_state`
 *	to a different entry in the lookup table
 */
static inline void LEX_go_to_state(LEX_fsm_state_id_t state_id)
{
	ASSERT(state_id < LEX_FSM_STATE_ID_NUM_STATES);
	if (state_id == LEX_FSM_STATE_ID_WAIT_SCANNING_DELIM)
	{
		lex_info.u32_num_statements++;
	}
	lex_info.p_state = &p_fsm_states[state_id];
}

static void LEX_flush_to_token(void)
{

	if (lex_info.u8_current_lexeme_index == 0)
	{
		return;
	}

	// Double our token buffer if it's full
	if (lex_info.token_list.u32_num_tokens == lex_info.u32_token_buffer_capacity)
	{
		lex_info.u32_token_buffer_capacity *= 2;
		lex_info.token_list.p_tokens = realloc(lex_info.token_list.p_tokens, sizeof(LEX_token_t) * lex_info.u32_token_buffer_capacity);
		ASSERT(lex_info.token_list.p_tokens);
	}

	// Build the token
	lex_info.current_token.type = LEX_token_type_from_lexeme();
	lex_info.current_token.u32_column = lex_info.u32_column;
	lex_info.current_token.u32_row = lex_info.u32_row;
	strncpy(lex_info.current_token.pc_lexeme, lex_info.p_current_lexeme, LEX_MAX_LEXEME_SIZE);

	// Append the token
	memcpy(&lex_info.token_list.p_tokens[lex_info.token_list.u32_num_tokens++], &lex_info.current_token, sizeof(LEX_token_t));

	// Zero lexeme tracking members
	lex_info.u8_current_lexeme_index = 0;
	memset(lex_info.p_current_lexeme, 0, LEX_MAX_LEXEME_SIZE);
}

static LEX_token_type_t LEX_token_type_from_lexeme(void)
{
    LEX_token_type_t type = LEX_TOKEN_TYPE_UNKNOWN;
    bool b_is_numeric = true;

    ASSERT(lex_info.u8_current_lexeme_index > 0);
    
    // We can get some of the easy ones out of the way here
    if (strlen(lex_info.p_current_lexeme) == 1)
    {
        char c = lex_info.p_current_lexeme[0];

        switch (c)
        {
            case '0' ... '9':   {type = LEX_TOKEN_TYPE_INT_LITERAL; 	break;}
            case '(':           {type = LEX_TOKEN_TYPE_OPEN_PAREN;  	break;}
            case ')':           {type = LEX_TOKEN_TYPE_CLOSE_PAREN; 	break;}
            case '=':           {type = LEX_TOKEN_TYPE_OP_ASSIGNMENT; 	break;}
            case '+':           {type = LEX_TOKEN_TYPE_OP_ADD;      	break;}
            case '-':           {type = LEX_TOKEN_TYPE_OP_SUBTRACT; 	break;}
            case '*':           {type = LEX_TOKEN_TYPE_OP_MULTIPLY; 	break;}
            case '/':           {type = LEX_TOKEN_TYPE_OP_DIVIDE;   	break;}
            case ';':           {type = LEX_TOKEN_TYPE_DELIM;       	break;}
            default:            {type = LEX_TOKEN_TYPE_UNKNOWN;     	break;}
        }
    }
    else
    {   
		// If any character is not a number, the lexeme is not an int literal
        for (uint32_t i = 0; i < strlen(lex_info.p_current_lexeme); ++i)
        {
            if (!isdigit(lex_info.p_current_lexeme[i]))
            {
                b_is_numeric = false;
                break;
            }
        }
        if (b_is_numeric)
        {
            type = LEX_TOKEN_TYPE_INT_LITERAL;
        }
        else
        {
			// If the leading char is alpha, the following chars can be alphanum/ underscore
			// TODO: check for invalid chars here
            if (isalpha(lex_info.p_current_lexeme[0]))
            {
				type = LEX_TOKEN_TYPE_IDENTIFIER;
            }
            else
            {
                type = LEX_TOKEN_TYPE_UNKNOWN;
            }
        }
    }

    return type;
}

static inline void LEX_push_to_current_lexeme(char c)
{
	lex_info.p_current_lexeme[lex_info.u8_current_lexeme_index++] = c;
	lex_info.p_current_lexeme[lex_info.u8_current_lexeme_index] = '\0';
}

static bool LEX_handle_STATE_START(void)
{
	bool b_res = false;
	char c = lex_info.c_current_char;
	lex_info.u32_column++;

	if (LEX_SCANNING_WHITESPACE(c))
	{
		LEX_flush_to_token();

		if (LEX_SCANNING_NEWLINE(c))
		{
			lex_info.u32_row++;
			lex_info.u32_column = 0;
		}
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_WHITESPACE);
		b_res = true;
	}
	if (LEX_SCANNING_DELIM(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_DELIM);
		b_res = true;
	}
	if (LEX_SCANNING_ALPHA(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_IDENTIFIER);
		b_res = true;
	}
	if (LEX_SCANNING_NUMBER(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_NUMBER);
		b_res = true;
	}
	if (LEX_SCANNING_OPERATOR(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_OPERATOR);
		b_res = true;
	}
	if (LEX_SCANNING_CONTROL_CHAR(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_CONTROL_CHAR);
		b_res = true;
	}

	return b_res;
}

static bool LEX_handle_STATE_WAIT_SCANNING_IDENTIFIER(void)
{
	bool b_res = false;
	char c = lex_info.c_current_char;
	lex_info.u32_column++;

	if (LEX_SCANNING_WHITESPACE(c))
	{
		LEX_flush_to_token();

		if (LEX_SCANNING_NEWLINE(c))
		{
			lex_info.u32_row++;
			lex_info.u32_column = 0;
		}
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_WHITESPACE);
		b_res = true;
	}
	if (LEX_SCANNING_DELIM(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_DELIM);
		b_res = true;
	}
	if (LEX_SCANNING_ALPHA(c))
	{		
		LEX_push_to_current_lexeme(c);		
		b_res = true;
	}
	if (LEX_SCANNING_NUMBER(c))
	{
		LEX_push_to_current_lexeme(c);
		b_res = true;
	}
	if (LEX_SCANNING_OPERATOR(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_OPERATOR);
		b_res = true;
	}
	if (LEX_SCANNING_CONTROL_CHAR(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_CONTROL_CHAR);
		b_res = true;
	}

	return b_res;
}

static bool LEX_handle_STATE_WAIT_SCANNING_NUMBER(void)
{
	bool b_res = false;
	char c = lex_info.c_current_char;
	lex_info.u32_column++;

	if (LEX_SCANNING_WHITESPACE(c))
	{
		LEX_flush_to_token();

		if (LEX_SCANNING_NEWLINE(c))
		{
			lex_info.u32_row++;
			lex_info.u32_column = 0;
		}
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_WHITESPACE);
		b_res = true;
	}
	if (LEX_SCANNING_DELIM(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_DELIM);
		b_res = true;
	}
	if (LEX_SCANNING_ALPHA(c))
	{
		// LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_IDENTIFIER);
		b_res = true;
	}
	if (LEX_SCANNING_NUMBER(c))
	{
		LEX_push_to_current_lexeme(c);
		b_res = true;
	}
	if (LEX_SCANNING_OPERATOR(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_OPERATOR);
		b_res = true;
	}
	if (LEX_SCANNING_CONTROL_CHAR(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_CONTROL_CHAR);
		b_res = true;
	}

	return b_res;
}

static bool LEX_handle_STATE_WAIT_SCANNING_OPERATOR(void)
{
	bool b_res = false;
	char c = lex_info.c_current_char;
	lex_info.u32_column++;

	if (LEX_SCANNING_WHITESPACE(c))
	{
		LEX_flush_to_token();

		if (LEX_SCANNING_NEWLINE(c))
		{
			lex_info.u32_row++;
			lex_info.u32_column = 0;
		}
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_WHITESPACE);
		b_res = true;
	}
	if (LEX_SCANNING_DELIM(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_DELIM);
		b_res = true;
	}
	if (LEX_SCANNING_NUMBER(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_NUMBER);
		b_res = true;
	}
	if (LEX_SCANNING_OPERATOR(c))
	{
		LEX_push_to_current_lexeme(c);
		b_res = true;
	}
	if (LEX_SCANNING_CONTROL_CHAR(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_CONTROL_CHAR);
		b_res = true;
	}

	return b_res;
}

static bool LEX_handle_STATE_WAIT_SCANNING_CONTROL_CHAR(void)
{
	bool b_res = false;
	char c = lex_info.c_current_char;
	lex_info.u32_column++;

	if (LEX_SCANNING_WHITESPACE(c))
	{
		LEX_flush_to_token();

		if (LEX_SCANNING_NEWLINE(c))
		{
			lex_info.u32_row++;
			lex_info.u32_column = 0;
		}
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_WHITESPACE);
		b_res = true;
	}
	if (LEX_SCANNING_DELIM(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_DELIM);
		b_res = true;
	}
	if (LEX_SCANNING_NUMBER(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_NUMBER);
		b_res = true;
	}
	if (LEX_SCANNING_OPERATOR(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		LEX_go_to_state(LEX_FSM_STATE_ID_WAIT_SCANNING_OPERATOR);
		b_res = true;
	}
	if (LEX_SCANNING_CONTROL_CHAR(c))
	{
		LEX_flush_to_token();
		LEX_push_to_current_lexeme(c);
		b_res = true;
	}

	return b_res;
}

static const char * LEX_get_token_type_descriptor (const LEX_token_type_t k_token_type)
{
	ASSERT(k_token_type < LEX_TOKEN_TYPE_NUM_TYPES)
	return pk_token_type_descriptors[k_token_type];
}

static void LEX_fsm_report (void)
{
	char pc_buff[3] = {lex_info.c_current_char, '\0', '\0'};

	// This makes debugging whitespace characters a bit clearer
	switch (lex_info.c_current_char)
	{
		case '\n': {strncpy(pc_buff, "\\n", 3); break;}
		case '\t': {strncpy(pc_buff, "\\t", 3); break;}
	}

	LEX_DBG("Received char [%s] in state %-30s\t[row: %03u, column %03u]\n", 
						pc_buff, 
						lex_info.p_state->descriptor, 
						lex_info.u32_row, 
						lex_info.u32_column);
}

static void LEX_restore_defaults (void)
{
	lex_info.u8_current_lexeme_index = 0;
	lex_info.u32_num_statements = 0;
	memset(lex_info.p_current_lexeme, 0, LEX_MAX_LEXEME_SIZE);
	lex_info.token_list.u32_num_tokens = 0;
	lex_info.u32_token_buffer_capacity = LEX_INITIAL_TOKEN_BUFFER_SIZE;
	lex_info.p_state = &p_fsm_states[LEX_FSM_STATE_ID_START];	
}
