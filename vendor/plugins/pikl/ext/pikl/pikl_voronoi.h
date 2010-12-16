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
//ボロノイの母点管理用(1つの母点から隣接する母点の個数)
#define VORONOI_CNT 10	//根拠はない

struct VORONOI {
	int x, y;				//母点座標
	int id;					//管理用ID
	long color[PKL_CHANNEL];	//ターゲット領域の色累積領域
	int count;				//累積ピクセル数
	int next[VORONOI_CNT];	//隣接する母点のID
};

#endif
