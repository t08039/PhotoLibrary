#ifndef _PIKL_ENHANCE_
#define _PIKL_ENHANCE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pikl.h"
#include "pikl_private.h"

//unsharp target pixel
#define RADIUS 1

typedef struct {
	int max, min;
	long data[PKL_COLOR];
} PKL_HISTGRAM;

#endif
