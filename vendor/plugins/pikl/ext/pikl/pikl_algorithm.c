#include "pikl_algorithm.h"

//=================================================================================
// sample_select
//=================================================================================
void *sample_select(PKL_SAMPLE sample)
{
	switch(sample){
		case PKL_SAMPLE_NN:
			return pkl_nearestneighbor;

		case PKL_SAMPLE_BL:
			return pkl_bilinear;

		case PKL_SAMPLE_BC:
		case PKL_SAMPLE_PA:
		case PKL_SAMPLE_LZ:
			return pkl_bicubic;
		
		case PKL_SAMPLE_ERROR:
		default:
			return NULL;
	}
	return NULL;
}

//=================================================================================
// pkl_nearestneighbor
//=================================================================================
void pkl_nearestneighbor(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color)
{
	int sx, sy;
	
	//sx = fx < 0.0f ? (int)(fx-0.5f) : (int)(fx+0.5f);
	//sy = fy < 0.0f ? (int)(fy-0.5f) : (int)(fy+0.5f);
	//sx = (int)floor(fx);
	//sy = (int)floor(fy);
	sx =(int)fx;
	sy =(int)fy;
	
	if(sx>=0 && sx<pkl->width && sy>=0 && sy<pkl->height)
		memcpy(dst, &pkl->image[(sy*pkl->width+sx)*pkl->channel], pkl->channel);
	else
		memcpy(dst, color, pkl->channel);
}

//=================================================================================
// pkl_bilinear
//	+----+----+
//	| d0 | d1 |
//	+----+----+
//	| d2 | d3 |
//	+----+----+
//	fx, fy: 計算によって得られた取得座標(double)
//=================================================================================
void pkl_bilinear(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color)
{
	double dx, dy, weight[4], pixel[PKL_CHANNEL];
	int sx, sy, wx, wy, k;
	unsigned char *p;
	
	//取得ピクセル(小数部を切り捨てることで、実イメージ空間の座標にする)
	sx = (int)floor(fx);
	sy = (int)floor(fy);

	//-1まで許容するのは、上位方向の範囲を下位方向と合わせるため
	//if(sx<0 || sx>=pkl->width || sy<0 || sy>=pkl->height){
	if(sx<-1 || sx>=pkl->width || sy<-1 || sy>=pkl->height){
		memcpy(dst, color, pkl->channel);
		return;
	}

	//取得ピクセルとfx,fyとの距離(<1.0)
	dx = fx - sx;
	dy = fy - sy;
	
	//距離ウェイト
	weight[0] = (1.0-dx)*(1.0-dy);
	weight[1] = dx*(1.0-dy);
	weight[2] = (1.0-dx)*dy;
	weight[3] = dx*dy;
	
	memset(pixel, 0, sizeof(double)*PKL_CHANNEL);
	for(wy=0; wy<2; wy++){
		for(wx=0; wx<2; wx++){
			if(sx+wx>=0 && sx+wx<pkl->width && sy+wy>=0 && sy+wy<pkl->height)
				p = &pkl->image[((sy+wy)*pkl->width+sx+wx)*pkl->channel];
			else
				p = color;
			
			for(k=0; k<pkl->channel; k++)
				pixel[k] += p[k] * weight[wy*2+wx];
		}
	}
	
	for(k=0; k<pkl->channel; k++)
		dst[k] = PKL_COLOR_CLIP(pixel[k]);
}

//=================================================================================
// pkl_bicubic
//	+---+---+---+---+
//	|   |   |   |   |
//	+---+---+---+---+
//	|   | * |   |   |
//	+---+---+---+---+
//	|   |   |   |   |
//	+---+---+---+---+
//	|   |   |   |   |
//	+---+---+---+---+
//=============================================================================
void pkl_bicubic(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color)
{
	unsigned char *p;
	int k, px, py, sx, sy;
	double wx, wy, dx, dy;
	double data[PKL_CHANNEL];

	sx = (int)(floor(fx));
	sy = (int)(floor(fy));
	
	//-1まで許容するのは、上位方向の範囲を下位方向と合わせるため
	if(sx<-1 || sx>=pkl->width || sy<-1 || sy>=pkl->height){
		memcpy(dst, color, pkl->channel);
		return;
	}
	
	memset(&data, 0, sizeof(data));
	for(py=sy-1; py<=sy+2; py++){
		for(px=sx-1; px<=sx+2; px++){
			dx = fabs(fx-px);
			dy = fabs(fy-py);
			wx = (dx<1) ? (dx-1)*(dx*dx-dx-1) : -(dx-1)*(dx-2)*(dx-2);
			wy = (dy<1) ? (dy-1)*(dy*dy-dy-1) : -(dy-1)*(dy-2)*(dy-2);
			
			if(px>=0 && px<pkl->width && py>=0 && py<pkl->height)
				p = &pkl->image[(py*pkl->width+px)*pkl->channel];
			else
				p = color;
			
			for(k=0; k<pkl->channel; k++)
				data[k] += p[k]*wx*wy;
		}
	}
	for(k=0; k<pkl->channel; k++)
		dst[k] = PKL_COLOR_CLIP(data[k]);
}

