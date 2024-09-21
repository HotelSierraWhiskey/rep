#include "io_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#ifdef DEBUG_IO
#define IO_DBG(fmt, ...)				printf("IO:\t"fmt, ##__VA_ARGS__)
#else
#define IO_DBG(fmt, ...)				asm("nop");
#endif

/****************************************************************************************************
 *	S T A T I C   V A R I A B L E S
 ****************************************************************************************************/

static IO_HANDLER_source_info_t io_source_info;

/****************************************************************************************************
 *	P U B L I C   F U N C T I O N   D E F I N I T I O N S
 ****************************************************************************************************/

void IO_HANDLER_load_source_file(const char * kpc_fname)
{
	IO_DBG("Loading file: %s\n", kpc_fname);

	FILE * 		file = fopen(kpc_fname, "rb");
	int32_t 	i32_file_size;

	fseek(file, 0, SEEK_END);

	i32_file_size = ftell(file);

	rewind(file);

	io_source_info.pc_source_buffer = (char *)malloc(i32_file_size + 1);

	io_source_info.u32_size = fread(io_source_info.pc_source_buffer, 1, i32_file_size, file);

	io_source_info.pc_source_buffer[i32_file_size] = '\0';

	IO_DBG("Done\n");
}

const IO_HANDLER_source_info_t * IO_HANDLER_get_source_info(void)
{
	return &(io_source_info);
}
