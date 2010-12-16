#ifndef _PIKL_PNG_
#define _PIKL_PNG_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <png.h>

#include "pikl.h"
#include "pikl_private.h"

int load_png(PKLImage pkl, FILE *image);
int save_png(PKLImage pkl, FILE *image);

#endif

