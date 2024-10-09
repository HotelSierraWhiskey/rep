#ifndef CODE_GEN_H
#define CODE_GEN_H

#include "common.h"
#include "parse.h"

typedef enum
{
	CODE_GEN_SCRATCH_REGISTER_0 = 0,
	CODE_GEN_SCRATCH_REGISTER_1,
	CODE_GEN_SCRATCH_REGISTER_2,
	CODE_GEN_SCRATCH_REGISTER_3,
	CODE_GEN_SCRATCH_REGISTER_4,
	CODE_GEN_SCRATCH_REGISTER_5,
	//////////////////////////////
	CODE_GEN_SCRATCH_REGISTER_NUM_SCRATCH_REGISTERS
} CODE_GEN_scratch_register_id_t;

void CODE_GEN_traverse_tree	(PARSE_node_t *p_root);
void CODE_GEN_handle_node	(PARSE_node_type_t expected_type, PARSE_node_t * p_node);

#endif