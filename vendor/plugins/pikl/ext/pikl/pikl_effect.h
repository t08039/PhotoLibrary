#ifndef _PIKL_EFFECT_
#define _PIKL_EFFECT_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pikl.h"
#include "pikl_private.h"

struct PKL_FILTER {
	int type;
	int matrix[9];
};

struct PKL_FILTER emboss_filter[] =
{
	{ PKL_EMBOSS_EMBOSS,    { 0,  0, -1,
							  0,  0,  0,
							  1,  0,  0 }},
	{ PKL_EMBOSS_LAPLACIAN, {-1, -1, -1,
							 -1,  8, -1,
							 -1, -1, -1 }},
	{ PKL_EMBOSS_HEAVY,     { 2,  0,  0,
							  0, -1,  0,
							  0,  0, -1 }},
	{ PKL_EMBOSS_LIGHT,     { 0,  0,  0,
							  0,  1,  0,
							  0,  0, -1 }},
};

struct PKL_FILTER focus_filter[] =
{
	{ PKL_FOCUS_DETAIL,    { 0, -1,  0,
							-1, 10, -1,
							 0, -1,  0 }},
	{ PKL_FOCUS_EDGES,     {-1, -1, -1,
							-1,  9, -1,
							-1, -1, -1 }},
	{ PKL_FOCUS_FOCUS,     {-1,  0, -1,
							 0,  7,  0,
							-1,  0, -1 }},
};

#endif
