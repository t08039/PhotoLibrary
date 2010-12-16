#ifndef _PIKL_ROTATE_
#define _PIKL_ROTATE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pikl.h"
#include "pikl_private.h"
#include "pikl_algorithm.h"

typedef struct {
	double rcos, rsin;
	int width, height;
	int sx, sy;
	int ex, ey;
	unsigned char back[PKL_CHANNEL];
} PIKL_ROTATE;

#endif
