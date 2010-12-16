#ifndef _PIKL_PRIVATE_
#define _PIKL_PRIVATE_

#ifdef DEBUG
#include "dbg/dbg.h"
#endif

#define PKL_MINMAX(a,b,i) (((a>=i)?a:((b<=i)?b:i)))
#define PKL_MIN(a,b) ((a<b)?a:b)
#define PKL_MAX(a,b) ((a>b)?a:b)
#define PKL_COLOR_CLIP(v) v<0?0:v>255?255:v
#define PKL_CHANNEL 4
#define PKL_COLOR 256
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif
#define PKL_RESOLUTION 72.0

//=============================================================================
// PKLImage
//=============================================================================
struct _PKLImage {
	int width;
	int height;
	PKL_COLOR_SPACE color;
	int channel;
	unsigned char *image;
	PKL_FORMAT format;
	int compress;
	double resh, resv;	//resolution(dpi)
};

//=============================================================================
// PKLColor
//=============================================================================
struct _PKLColor {
	unsigned char color[PKL_CHANNEL];
};
#endif
