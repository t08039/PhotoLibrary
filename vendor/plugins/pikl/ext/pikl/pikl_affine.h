#ifndef _PIKL_AFFINE_
#define _PIKL_AFFINE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pikl.h"
#include "pikl_private.h"
#include "pikl_algorithm.h"

typedef struct {
	double x, y;
} point2;

typedef struct {
	double a,b,c,d,e,f;
} matrix;

#endif
