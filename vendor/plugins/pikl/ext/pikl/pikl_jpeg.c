#include "pikl_jpeg.h"

static void pkljpeg_error_exit(j_common_ptr cinfo);
static int color_p2j(int color);
static int color_j2p(int color);
static int compress_p2j(int level);
static double resolution_j2p(int unit, int density);

//=============================================================================
// pkljpeg_error_exit
//=============================================================================
static void pkljpeg_error_exit(j_common_ptr cinfo)
{
	struct err_mgr *jp_err = (struct err_mgr *)cinfo->err;
	longjmp(jp_err->setjmp_buffer, 1);
}

//=============================================================================
// load_jpeg
//=============================================================================
int load_jpeg(PKLImage pkl, FILE *image)
{
	struct jpeg_decompress_struct cinfo;
	struct err_mgr jerr;
	unsigned char *p;
	
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = pkljpeg_error_exit;
	
	if( setjmp(jerr.setjmp_buffer) ){
		jpeg_destroy_decompress(&cinfo);
		return(1);
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, image);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	
	pkl->width   = cinfo.output_width;
	pkl->height  = cinfo.output_height;
	pkl->color   = color_j2p(cinfo.out_color_space);
	pkl->channel = cinfo.output_components;
	pkl->resh = resolution_j2p(cinfo.density_unit, cinfo.X_density);
	pkl->resv = resolution_j2p(cinfo.density_unit, cinfo.Y_density);
	if(pkl->color == PKL_UNKNOWN) return(1);
	
	pkl->image = malloc(cinfo.output_height * cinfo.output_width * cinfo.output_components);
	if(!pkl->image) return(1);
	
	p = pkl->image;
	while(cinfo.output_scanline < cinfo.output_height){
		jpeg_read_scanlines(&cinfo, &p, 1);
		p += (cinfo.output_width * cinfo.output_components);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return(0);
}

//=================================================================================
// resolution_j2p
//=================================================================================
static double resolution_j2p(int unit, int density)
{
	if(density==0)
		return PKL_RESOLUTION;
	
	switch(unit){
		case 1:			//dots/inch
			return (double)density;
		case 2:			//dots/cm
			return (double)density*2.54 + 0.5;
		default:
			;;
	}
	return PKL_RESOLUTION;
}

//=============================================================================
// save_jpg
//=============================================================================
int save_jpeg(PKLImage pkl, FILE *image)
{
	struct jpeg_compress_struct oinfo;
	struct err_mgr jerr;
	JSAMPROW   row_pointer[1];
	int i;

	if(pkl->color == PKL_UNKNOWN) return(1);
	
	oinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = pkljpeg_error_exit;
	
	if( setjmp(jerr.setjmp_buffer) ){
		jpeg_destroy_compress(&oinfo);
		return(1);
	}

	jpeg_create_compress(&oinfo);
	jpeg_stdio_dest(&oinfo, image);

	oinfo.image_width      = pkl->width;
	oinfo.image_height     = pkl->height;
	oinfo.in_color_space   = color_p2j(pkl->color);
	oinfo.input_components = pkl->channel;
	jpeg_set_defaults(&oinfo);
	jpeg_set_quality(&oinfo, compress_p2j(pkl->compress), TRUE);
	oinfo.write_JFIF_header= TRUE;
	oinfo.density_unit     = 1;
	oinfo.X_density = (int)pkl->resh;
	oinfo.Y_density = (int)pkl->resv;
	
	jpeg_start_compress(&oinfo, TRUE);
	
	for(i=0; i<pkl->height ; i++){
		row_pointer[0] = &pkl->image[i*pkl->width*pkl->channel];
		jpeg_write_scanlines(&oinfo, row_pointer, 1);
	}
	
	jpeg_finish_compress(&oinfo);
	jpeg_destroy_compress(&oinfo);
	

	return(0);
}

//=============================================================================
// color_p2j
//=============================================================================
static int color_p2j(int color)
{
	switch(color){
		case PKL_GRAYSCALE:
			return JCS_GRAYSCALE;
		case PKL_RGB:
			return JCS_RGB;
		//case PKL_YCbCr:
		//	return JCS_YCbCr;
		case PKL_CMYK:
			return JCS_CMYK;
		//case PKL_YCCK:
		//	return JCS_YCCK;
		//case PKL_RGBA:
		default:
			return JCS_UNKNOWN;
	}
	return JCS_UNKNOWN;
}

//=============================================================================
// color_j2p
//=============================================================================
static int color_j2p(int color)
{
	switch(color){
		case JCS_GRAYSCALE:
			return PKL_GRAYSCALE;
		case JCS_RGB:
			return PKL_RGB;
		//case JCS_YCbCr:
		//	return PKL_CMYK;
		case JCS_CMYK:
			return PKL_CMYK;
		//case JCS_YCCK:
		//	return PKL_YCCK;
		default:
			return PKL_UNKNOWN;
	}
	return PKL_UNKNOWN;
}

//=============================================================================
// compress_p2j
//=============================================================================
static int compress_p2j(int level)
{
	int comp;
	
	if(level < 0) return(90);
	comp = 100 - (level*10);
	if(comp <= 0) return(1);
	return(comp);
}
