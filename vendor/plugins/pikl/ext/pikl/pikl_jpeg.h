#ifndef _PIKL_JPEG_
#define _PIKL_JPEG_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <jpeglib.h>

#include "pikl.h"
#include "pikl_private.h"

/* libjpeg error handler */
struct err_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

int load_jpeg(PKLImage pkl, FILE *image);
int save_jpeg(PKLImage pkl, FILE *image);

#endif
