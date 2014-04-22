#include "io_constants.h"

/* NULL_STREAM has external linkage and is defined in this file */
Stream null_stream = { (Device *) 0, NULL_DEVICE, (char *) 0, (char *) 0 };
Stream *NULL_STREAM = &null_stream;
