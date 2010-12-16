#ifndef _PIKL_ALGORITHM_
#define _PIKL_ALGORITHM_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pikl.h"
#include "pikl_private.h"

//サンプリングセレクター
void *sample_select(PKL_SAMPLE sample);

//ニアレストネイバー
void pkl_nearestneighbor(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);

//バイリニア
void pkl_bilinear(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);

//バイキュービック
void pkl_bicubic(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);

#endif
