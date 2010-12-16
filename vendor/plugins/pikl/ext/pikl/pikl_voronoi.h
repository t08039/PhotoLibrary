#ifndef _PIKL_VORONOI_
#define _PIKL_VORONOI_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pikl.h"
#include "pikl_private.h"


//=============================================================================
// voronoi_zone datatype
//=============================================================================
struct VoronoiPoint {
	int x, y;
	long color[PKL_CHANNEL];
	unsigned char ave[PKL_CHANNEL];
	int count;
};

//=============================================================================
// voronoi_count datatype
//=============================================================================
//�{���m�C�̕�_�Ǘ��p(1�̕�_����אڂ����_�̌�)
#define VORONOI_CNT 10	//�����͂Ȃ�

struct VORONOI {
	int x, y;				//��_���W
	int id;					//�Ǘ��pID
	long color[PKL_CHANNEL];	//�^�[�Q�b�g�̈�̐F�ݐϗ̈�
	int count;				//�ݐσs�N�Z����
	int next[VORONOI_CNT];	//�אڂ����_��ID
};

#endif
