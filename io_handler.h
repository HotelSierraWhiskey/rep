#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "common.h"
#include "status.h"

/****************************************************************************************************
 *	D E F I N E S
 ****************************************************************************************************/

#define IO_REP_EXT_LENGTH				(4)
#define IO_MIN_REP_FILE_NAME_LENGTH		(IO_REP_EXT_LENGTH + 1)	// For example: a.rep
#define IO_MAX_REP_FILE_NAME_LENGTH		(255)

/****************************************************************************************************
 *	T Y P E D E F S
 ****************************************************************************************************/

typedef struct _IO_HANDLER_source_info
{
	char		pc_source_file_name[IO_MAX_REP_FILE_NAME_LENGTH];
	char * 		pc_source_buffer;
	uint32_t	u32_size;
} IO_HANDLER_source_info_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

STATUS_t 							IO_HANDLER_load_source_file 	(const char * kpc_fname);
const IO_HANDLER_source_info_t * 	IO_HANDLER_get_source_info		(void);


#endif