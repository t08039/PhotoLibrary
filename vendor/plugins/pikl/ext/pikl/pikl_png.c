#include "pikl_png.h"

static int color_pn2pk(int color);
static int color_pk2pn(int color);
static int pkl_png_channels(int color);
static int pkl_png_depth(int color);
static int compress_pk2pn(int level);
static double resolution_p2p(int unit, int density);

//=============================================================================
// load_png
//=============================================================================
int load_png(PKLImage pkl, FILE *image)
{
	png_structp png_ptr;
	png_infop   info_ptr;
	int i;
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr) return(1);
	
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr){
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return(1);
	}

	if( setjmp(png_jmpbuf(png_ptr)) ){
		png_destroy_info_struct(png_ptr, &info_ptr);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return(1);
	}

	png_init_io(png_ptr, image);
	png_read_info(png_ptr, info_ptr);
	pkl->width   = info_ptr->width;
	pkl->height  = info_ptr->height;
	pkl->color   = color_pn2pk(info_ptr->color_type);
	pkl->channel = pkl_png_channels(pkl->color);
	pkl->resh = resolution_p2p(info_ptr->phys_unit_type, info_ptr->x_pixels_per_unit);
	pkl->resv = resolution_p2p(info_ptr->phys_unit_type, info_ptr->y_pixels_per_unit);
	if(pkl->color == PKL_UNKNOWN) return(1);

	pkl->image = malloc(pkl->width * pkl->height * pkl->channel);
	if(!pkl->image) return(1);

	if(info_ptr->color_type==PNG_COLOR_TYPE_RGB_ALPHA  ||
	   info_ptr->color_type==PNG_COLOR_TYPE_GRAY_ALPHA || info_ptr->num_trans) png_set_strip_alpha(png_ptr);
	if(info_ptr->bit_depth<8 &&
	   info_ptr->color_type==PNG_COLOR_TYPE_GRAY) png_set_gray_1_2_4_to_8(png_ptr);
	if(info_ptr->bit_depth==16) png_set_strip_16(png_ptr);
	if(info_ptr->color_type==PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);

	for(i=0; i<pkl->height; i++)
		png_read_row(png_ptr, &pkl->image[i*pkl->width*pkl->channel], NULL);
	
	png_destroy_info_struct(png_ptr, &info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	
	return(0);
}

//=================================================================================
// resolution_p2p
//=================================================================================
static double resolution_p2p(int unit, int density)
{
	if(density==0)
		return PKL_RESOLUTION;
	
	switch(unit){
		case 0:		//ピクセル/不明な単位-->dpiに変換
			return (double)density;
		case 1:		//dots/m
			return (double)density/100.0*2.54+0.5;
		default:	//invalid
			;;
	}
	return PKL_RESOLUTION;
}

//===================================================================
// save_png
//===================================================================
int save_png(PKLImage pkl, FILE *image)
{
	png_structp png_ptr;
	png_infop   info_ptr;
	int  i;
	
	if(pkl->color!=PKL_RGB &&
	   pkl->color!=PKL_GRAYSCALE) return(1);
	
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr) return(1);
	
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr){
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		return(1);
	}
	
	if( setjmp(png_jmpbuf(png_ptr)) ){
		png_destroy_info_struct(png_ptr, &info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return(1);
	}
	
	png_init_io(png_ptr, image);
	info_ptr->width  = pkl->width;
	info_ptr->height = pkl->height;
	info_ptr->bit_depth  = pkl_png_depth(pkl->color);
	info_ptr->color_type = color_pk2pn(pkl->color);
	png_set_pHYs(png_ptr, info_ptr, (int)(pkl->resh*10000.0/254.0), (int)(pkl->resv*10000.0/254.0), PNG_RESOLUTION_METER);
	png_set_compression_level(png_ptr, compress_pk2pn(pkl->compress));
	png_write_info(png_ptr, info_ptr);
	
	for(i=0; i<pkl->height; i++)
		png_write_row(png_ptr, &pkl->image[i*pkl->width*pkl->channel]);
	png_write_end(png_ptr, info_ptr);
	png_destroy_info_struct(png_ptr, &info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return(0);
}

//=============================================================================
// color_pn2pk
//=============================================================================
static int color_pn2pk(int color)
{
	switch(color){
		case PNG_COLOR_TYPE_GRAY:
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			return(PKL_GRAYSCALE);
		case PNG_COLOR_TYPE_PALETTE:
		case PNG_COLOR_TYPE_RGB:
		case PNG_COLOR_TYPE_RGB_ALPHA:
			return(PKL_RGB);
		default:
			return(PKL_UNKNOWN);
	}
	return(PKL_UNKNOWN);
}

//=============================================================================
// pkl_png_channels
//=============================================================================
static int pkl_png_channels(int color)
{
	switch(color){
		//case PKL_BLACKWHITE:
		//	return ;
		case PKL_GRAYSCALE:
			return 1;
		case PKL_RGB:
			return 3;
		//case PKL_RGBA:
		//case PKL_CMYK:
		//	return 4;
		default:
			return 0;
	}
	return 0;
}

//=============================================================================
// pkl_png_depth
//=============================================================================
static int pkl_png_depth(int color)
{
	switch(color){
		//case PKL_BLACKWHITE:
		//	return 1;
		case PKL_GRAYSCALE:
		case PKL_RGB:
		//case PKL_CMYK:
		//case PKL_RGBA:
			return 8;
		default:
			return 0;
	}
	return 0;
}

//=============================================================================
// color_pk2pn
//=============================================================================
static int color_pk2pn(int color)
{
	switch(color){
		//case PKL_BLACKWHITE:
		//	return PNG_COLOR_TYPE_GRAY;
		case PKL_GRAYSCALE:
			return PNG_COLOR_TYPE_GRAY;
		case PKL_RGB:
			return PNG_COLOR_TYPE_RGB;
		//case PKL_CMYK:
		//case PKL_RGBA:
		default:
			return -1;
	}
	return -1;
}

//=============================================================================
// compress_pk2pn
//=============================================================================
static int compress_pk2pn(int level)
{
	if(level < 0) return(Z_BEST_COMPRESSION);
	if(level > 9) return(9);
	return(level);
}
