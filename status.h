#ifndef STATUS_H
#define STATUS_H

typedef enum
{
	// Common statuses
	STATUS_OK = 0,
	STATUS_FAILED,
	STATUS_MEMORY_ERROR,
	STATUS_UNINITIALIZED_ERROR,

	// IO statuses
	STATUS_FILE_ERROR,
	STATUS_FILE_NOT_FOUND_ERROR,
	STATUS_INVALID_FILE_ERROR,
	STATUS_EMTPY_FILE,
} STATUS_t;

#endif // STATUS_H