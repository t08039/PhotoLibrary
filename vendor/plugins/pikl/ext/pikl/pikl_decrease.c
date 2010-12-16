#include "pikl_decrease.h"

static void gray_normal(PKLImage pkl, unsigned char *gray);
static void gray_median(PKLImage pkl, unsigned char *gray);
static void gray_average(PKLImage pkl, unsigned char *gray);

//=============================================================================
// pkl_gray
//=============================================================================
PKLExport int pkl_gray(PKLImage pkl, PKL_GRAYTYPE type)
{
	unsigned char *gray;
	
	if(pkl->color!=PKL_RGB) return(1);
	
	gray = malloc(pkl->height * pkl->width);
	if(!gray) return(1);
	
	switch(type){
		case PKL_GRAY_NRM:
			gray_normal(pkl, gray);
			break;
		case PKL_GRAY_MED:
			gray_median(pkl, gray);
			break;
		case PKL_GRAY_AVE:
			gray_average(pkl, gray);
			break;
		default:
			return(1);
	}
	
	free(pkl->image);
	pkl->image = gray;
	pkl->channel=1;
	pkl->color=PKL_GRAYSCALE;
	return(0);
}

//=================================================================================
// gs_nrm(加重平均法)
//=================================================================================
static void gray_normal(PKLImage pkl, unsigned char *gray)
{
	int i, j;
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			gray[i*pkl->width+j] =
					pkl->image[(i*pkl->width+j)*pkl->channel  ] * 0.298912 +
					pkl->image[(i*pkl->width+j)*pkl->channel+1] * 0.586611 +
					pkl->image[(i*pkl->width+j)*pkl->channel+2] * 0.114478;
		}
	}
}

//=================================================================================
// gs_med(中間値法)
//=================================================================================
static void gray_median(PKLImage pkl, unsigned char *gray)
{
	int i, j;
	unsigned char mx, mn;
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			mx = pkl->image[(i*pkl->width+j)*pkl->channel];
			mx = PKL_MAX(pkl->image[(i*pkl->width+j)*pkl->channel+1], mx);
			mx = PKL_MAX(pkl->image[(i*pkl->width+j)*pkl->channel+2], mx);
			
			mn = pkl->image[(i*pkl->width+j)*pkl->channel];
			mn = PKL_MAX(pkl->image[(i*pkl->width+j)*pkl->channel+1], mn);
			mn = PKL_MAX(pkl->image[(i*pkl->width+j)*pkl->channel+2], mn);
			
			gray[i*pkl->width+j] = (mx + mn) * 0.5;
		}
	}
}

//=================================================================================
// gs_ave(単純平均法)
//=================================================================================
static void gray_average(PKLImage pkl, unsigned char *gray)
{
	int i, j;
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			gray[i*pkl->width+j] = 
				(pkl->image[(i*pkl->width+j)*pkl->channel  ] +
				 pkl->image[(i*pkl->width+j)*pkl->channel+1] +
				 pkl->image[(i*pkl->width+j)*pkl->channel+2]) / 3;
		}
	}
}

//=============================================================================
// pkl_rgb
//=============================================================================
PKLExport int pkl_rgb(PKLImage pkl)
{
	unsigned char *rgb, k;
	int i, j;
	
	if(pkl->color!=PKL_CMYK) return(1);
	
	rgb = malloc(pkl->height * pkl->width * 3);
	if(!rgb) return(1);
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			k = pkl->image[(i*pkl->width+j)*pkl->channel+3];
			rgb[(i*pkl->width+j)*3  ] = ~(255 - pkl->image[(i*pkl->width+j)*pkl->channel  ] + k);
			rgb[(i*pkl->width+j)*3+1] = ~(255 - pkl->image[(i*pkl->width+j)*pkl->channel+1] + k);
			rgb[(i*pkl->width+j)*3+2] = ~(255 - pkl->image[(i*pkl->width+j)*pkl->channel+2] + k);
		}
	}
	
	free(pkl->image);
	pkl->image = rgb;
	pkl->channel=3;
	pkl->color=PKL_RGB;
	return(0);
}

//=============================================================================
// pkl_2c
//=============================================================================
PKLExport int pkl_2c(PKLImage pkl, int threshold)
{
	int i;
	
	if(threshold<0 || threshold>255) return(1);
	
	switch(pkl->color){
		case PKL_CMYK:
			pkl_rgb(pkl);
		case PKL_RGB:
			pkl_gray(pkl, PKL_GRAY_AVE); //デフォルトで中間値法
		case PKL_GRAYSCALE:
			break;
		default:
			return(1);
	}
	
	for(i=0; i<pkl->height*pkl->width; i++)
		pkl->image[i] = pkl->image[i]<threshold ? 0 : 255;
	
	return(0);
}

//=============================================================================
// pkl_posterize
//=============================================================================
PKLExport int pkl_posterize(PKLImage pkl, int level)
{
	unsigned char *p, color[PKL_COLOR];
	int i, j, k;
	double d, n;
	
	if(level<1 && level>256) return(1);
	
	n = (256.0/(double)level);
	d = 255.0/(double)((double)level-1.0);

	for(i=0; i<level; i++)
		for(j=floor(i*n); j<floor((i+1)*n); j++)
			color[j]= PKL_COLOR_CLIP(d*i);
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			p = &pkl->image[(i*pkl->width+j)*pkl->channel];
			for(k=0; k<pkl->channel; k++,p++)
				*p = color[*p];
		}
	}
	return(0);
}

//=============================================================================
// pkl_cutcolor
//=============================================================================
PKLExport int pkl_cutcolor(PKLImage pkl, int level)
{
	unsigned char *p;
	int i, j, k;
	
	if(level<0 && level>256) return(1);
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			p = &pkl->image[(i*pkl->width+j)*pkl->channel];
			for(k=0; k<pkl->channel; k++){
				*p = (*p & level);
				p++;
			}
		}
	}
	return(0);
}

