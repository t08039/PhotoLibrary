#ifndef _PIKL_IO_
#define _PIKL_IO_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pikl.h"
#include "pikl_private.h"
#include "pikl_jpeg.h"
#include "pikl_bitmap.h"
#include "pikl_png.h"

#define PKL_HEADER_SIZE 8
struct PKL_HEADER {
	PKL_FORMAT format;
	int size;
	unsigned char mark[PKL_HEADER_SIZE];
};

static struct PKL_HEADER pklheader[] = {
	{ PKL_FORMAT_JPEG,   2, {0xFF, 0xD8} },
	{ PKL_FORMAT_PNG,    8, {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A} },
	{ PKL_FORMAT_BITMAP, 2, {0x42, 0x4D} },
};

#endif
