#ifndef _PIKL_BITMAP_
#define _PIKL_BITMAP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pikl.h"
#include "pikl_private.h"

#define BF_SIZE 14		//BITMAPFILEHEADER size
#define BI_SIZE 40		//BITMAPINFOHEADER size

int load_bitmap(PKLImage pkl, FILE *image);
int save_bitmap(PKLImage pkl, FILE *image);

#endif

