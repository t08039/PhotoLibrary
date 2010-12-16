#include "pikl_pixel.h"

static int check_offset(PKLImage pkl, int x, int y, PKL_COLOR_MODEL target);

//=============================================================================
// pkl_color_create
//=============================================================================
PKLExport PKLColor pkl_color_create(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
	PKLColor pcolor;
	
	pcolor = malloc(sizeof(struct _PKLColor));
	if(!pcolor) return(NULL);
	memset(pcolor, 0, sizeof(struct _PKLColor));
	
	pcolor->color[0] = a;
	pcolor->color[1] = b;
	pcolor->color[2] = c;
	pcolor->color[3] = d;
	return(pcolor);
}

//=============================================================================
// pkl_color_gray
//=============================================================================
PKLExport PKLColor pkl_color_gray(unsigned char gray)
{
	return pkl_color_create(gray, 0, 0, 0);
}

//=============================================================================
// pkl_color_rgb
//=============================================================================
PKLExport PKLColor pkl_color_rgb(unsigned char red, unsigned char green, unsigned char blue)
{
	return pkl_color_create(red, green, blue, 0);
}

//=============================================================================
// pkl_color_cmyk
//=============================================================================
PKLExport PKLColor pkl_color_cmyk(unsigned char c, unsigned char m, unsigned char y, unsigned char k)
{
	return pkl_color_create(c, m, y, k);
}

//=============================================================================
// pkl_color_close
//=============================================================================
PKLExport void pkl_color_close(PKLColor color)
{
	if(color) free(color);
}

//=============================================================================
// pkl_color_get
//=============================================================================
PKLExport unsigned char pkl_get_color(PKLImage pkl, int x, int y, PKL_COLOR_MODEL target)
{
	int offset=0;
	
	if((offset=check_offset(pkl, x, y, target)) == -1) return(0);
	return pkl->image[(y*pkl->width+x)*pkl->channel+offset];
}

//=============================================================================
// pkl_color_set
//=============================================================================
PKLExport int pkl_set_color(PKLImage pkl, int x, int y, PKL_COLOR_MODEL target, unsigned char color)
{
	int offset=0;

	if((offset=check_offset(pkl, x, y, target)) == -1) return(1);
	pkl->image[(y*pkl->width+x)*pkl->channel+offset]=color;
	return(0);
}

//=============================================================================
// check_offset
//=============================================================================
static int check_offset(PKLImage pkl, int x, int y, PKL_COLOR_MODEL target)
{
	if(x<0 || x>=pkl->width || y<0 || y>=pkl->height) return(-1);
	
	switch(pkl->color){
		case PKL_GRAYSCALE:
			if(target != PKL_GRAY) return(-1);
			return(0);
		case PKL_RGB:
			switch(target){
				case PKL_RED:   return(0);
				case PKL_GREEN: return(1);
				case PKL_BLUE:  return(2);
				default:
					return(-1);
			}
			break;
		case PKL_CMYK:
			switch(target){
				case PKL_CYAN:    return(0);
				case PKL_MAGENTA: return(1);
				case PKL_YELLOW:  return(2);
				case PKL_BLACK:   return(3);
				default:
					return(-1);
			}
			break;
		case PKL_UNKNOWN:
		case PKL_BLACKWHITE:
		case PKL_RGBA:
		case PKL_YCbCr:
		case PKL_YCCK:
		default:
			return(-1);
	}
	return(-1);
}

//=============================================================================
// pkl_pixel_get
//=============================================================================
PKLExport PKLColor pkl_get_pixel(PKLImage pkl, int x, int y)
{
	PKLColor pcolor;
	
	if(x<0 || x>=pkl->width || y<0 || y>=pkl->height) return(NULL);

	pcolor = malloc(sizeof(struct _PKLColor));
	if(!pcolor) return(NULL);
	memset(pcolor, 0, sizeof(struct _PKLColor));
	memcpy(pcolor->color, &pkl->image[(y*pkl->width+x)*pkl->channel], pkl->channel);
	return(pcolor);
}

//=============================================================================
// pkl_pixel_set
//=============================================================================
PKLExport int pkl_set_pixel(PKLImage pkl, int x, int y, PKLColor color)
{
	if(x<0 || x>=pkl->width || y<0 || y>=pkl->height) return(1);
	memcpy(&pkl->image[(y*pkl->width+x)*pkl->channel], color->color, pkl->channel);
	return(0);
}

//=============================================================================
// pkl_color
//=============================================================================
PKLExport unsigned char pkl_color(PKLColor color, PKL_COLOR_MODEL target)
{
	int offset;
	
	switch(target){
		case PKL_GRAY:
		case PKL_RED:
		case PKL_CYAN:
			offset=0;
			break;
		case PKL_GREEN:
		case PKL_MAGENTA:
			offset=1;
			break;
		case PKL_BLUE:
		case PKL_YELLOW:
			offset=2;
			break;
		case PKL_BLACK:
			offset=3;
			break;
		default:
			return(0);
	}
	return color->color[offset];
}
