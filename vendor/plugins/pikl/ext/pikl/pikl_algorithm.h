#ifndef _PIKL_ALGORITHM_
#define _PIKL_ALGORITHM_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pikl.h"
#include "pikl_private.h"

//�T���v�����O�Z���N�^�[
void *sample_select(PKL_SAMPLE sample);

//�j�A���X�g�l�C�o�[
void pkl_nearestneighbor(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);

//�o�C���j�A
void pkl_bilinear(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);

//�o�C�L���[�r�b�N
void pkl_bicubic(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);

#endif
