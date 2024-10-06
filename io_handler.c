#include "io_handler.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#ifdef DEBUG_IO
#define IO_DBG(fmt, ...)				printf(BOLD("IO:\t")fmt, ##__VA_ARGS__)
#define IO_GREEN(fmt, ...)				printf(BOLD(BRIGHT_GREEN("IO:\t"))fmt, ##__VA_ARGS__)
#define IO_WARN(fmt, ...)				printf(BOLD(BRIGHT_YELLOW("IO:\t"))fmt, ##__VA_ARGS__)
#define IO_ERR(fmt, ...)				printf(BOLD(BRIGHT_RED("IO:\t"))fmt, ##__VA_ARGS__)
#else
#define IO_DBG(fmt, ...)
#define IO_GREEN(fmt, ...)
#define IO_WARN(fmt, ...)
#define IO_ERR(fmt, ...)
#endif

#define IO_REP_EXT_LENGTH				(4)
#define IO_MIN_REP_FILE_NAME_LENGTH		(IO_REP_EXT_LENGTH + 1)	// For example: a.rep

/****************************************************************************************************
 *	S T A T I C   V A R I A B L E S
 ****************************************************************************************************/

static IO_HANDLER_source_info_t io_source_info;

/****************************************************************************************************
 *	P U B L I C   F U N C T I O N   D E F I N I T I O N S
 ****************************************************************************************************/

STATUS_t IO_HANDLER_load_source_file(const char * kpc_fname)
{
	uint32_t u32_file_name_length;

	if (kpc_fname == NULL)
	{
		IO_ERR("NULL input\n");
		return STATUS_FAILED;
	}

	u32_file_name_length = strlen(kpc_fname);

	if (u32_file_name_length < IO_MIN_REP_FILE_NAME_LENGTH)
	{
		IO_ERR("File name too short\n");
		return STATUS_FAILED;
	}

	if (strncmp(kpc_fname + (u32_file_name_length - IO_REP_EXT_LENGTH), ".rep", IO_REP_EXT_LENGTH) != 0)
	{
		IO_ERR("Invalid file\n");
		return STATUS_INVALID_FILE_ERROR;
	}

	IO_DBG("Loading file: %s\n", kpc_fname);

	FILE * 		file = fopen(kpc_fname, "rb");
	int32_t 	i32_file_size;

	io_source_info.pc_source_buffer = NULL;
	io_source_info.u32_size = 0;

	if (file == NULL)
	{
		IO_ERR("File not found: %s\n", kpc_fname);
		return STATUS_FILE_NOT_FOUND_ERROR;
	}

	if (fseek(file, 0, SEEK_END) != 0)
	{
		IO_ERR("File error\n");
		return STATUS_FILE_ERROR;
	}

	i32_file_size = ftell(file);

	if (i32_file_size == -1)
	{
		IO_ERR("File error\n");
		return STATUS_FILE_ERROR;
	}

	if (i32_file_size == 0)
	{
		IO_ERR("Empty File\n");
		return STATUS_EMTPY_FILE;
	}

	rewind(file);

	io_source_info.pc_source_buffer = (char *)malloc(i32_file_size + 1);

	if (io_source_info.pc_source_buffer == NULL)
	{
		IO_ERR("Memory error\n");
		return STATUS_MEMORY_ERROR;
	}

	io_source_info.u32_size = fread(io_source_info.pc_source_buffer, 1, i32_file_size, file);

	if (io_source_info.u32_size != i32_file_size)
	{
		IO_ERR("File error\n");
		return STATUS_FILE_ERROR;
	}

	IO_DBG("File size: %u bytes\n", io_source_info.u32_size);

	io_source_info.pc_source_buffer[i32_file_size] = '\0';

	IO_DBG(BOLD(BRIGHT_GREEN("Done\n")));

	return STATUS_OK;
}

const IO_HANDLER_source_info_t * IO_HANDLER_get_source_info(void)
{
	return &(io_source_info);
}
