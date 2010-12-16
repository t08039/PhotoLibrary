#include "pikl_io.h"

static int format_type(PKLImage pkl, FILE *image);

//=============================================================================
// pkl_canvas
//=============================================================================
PKLExport PKLImage pkl_canvas(int width, int height, PKL_COLOR_SPACE color, PKLColor backcolor)
{
	PKLImage pkl;
	PKL_COLOR_SPACE colorspace;
	int channel, i;
	unsigned char back[PKL_CHANNEL];
	
	switch(color){
		case PKL_GRAYSCALE:
			colorspace = PKL_GRAYSCALE;
			channel = 1;
			break;
		case PKL_RGB:
			colorspace = PKL_RGB;
			channel = 3;
			break;
		case PKL_CMYK:
			colorspace = PKL_CMYK;
			channel = 4;
			break;
		default:
			return(NULL);
	}
	
	pkl = malloc( sizeof(struct _PKLImage) );
	if(!pkl) return(NULL);
	memset(pkl, 0, sizeof(struct _PKLImage));
	
	pkl->image = malloc(width*height*channel);
	if(!pkl->image){
		free(pkl);
		return(NULL);
	}
	
	pkl->width = width;
	pkl->height = height;
	pkl->color = colorspace;
	pkl->channel = channel;
	pkl->format = PKL_FORMAT_ERROR;
	pkl->compress = -1;
	
	//for(i=0; i<pkl->channel; i++)
	//	back[i] = (backcolor>>((pkl->channel-i)*8) & 0xFF);
	if(backcolor)
		memcpy(back, backcolor->color, PKL_CHANNEL);
	else
		memset(back, 0xff, PKL_CHANNEL);
	
	for(i=0; i<pkl->width*pkl->height*pkl->channel; i+=pkl->channel)
		memcpy(&pkl->image[i], back, pkl->channel);
	return(pkl);
}

//=============================================================================
// pkl_open
//=============================================================================
PKLExport PKLImage pkl_open(const char *in)
{
	FILE *image;
	PKLImage pkl;
	
	image = fopen(in, "rb");
	if(!image) return(NULL);
	
	pkl = pkl_fdopen(image);
	
	fclose(image);
	return(pkl);
}

//=================================================================================
// pkl_fdopen
//=================================================================================
PKLExport PKLImage pkl_fdopen(FILE *in)
{
	PKLImage pkl;
	int result;
	
	pkl = malloc( sizeof(struct _PKLImage) );
	if(!pkl) return(NULL);
	memset(pkl, 0, sizeof(struct _PKLImage));
	
	pkl->compress = -1;
	
	format_type(pkl, in);

	switch(pkl->format){
		case PKL_FORMAT_JPEG:
			result = load_jpeg(pkl, in);
			break;
		case PKL_FORMAT_PNG:
			result = load_png(pkl, in);
			break;
		case PKL_FORMAT_BITMAP:
			result = load_bitmap(pkl, in);
			break;
		default:
			result = 1;
	}
	
	if(result){
		free(pkl);
		pkl=NULL;
	}
	return(pkl);
}

//=============================================================================
// pkl_close
//=============================================================================
PKLExport void pkl_close(PKLImage pkl)
{
	if(!pkl) return;
	if(pkl->image) free(pkl->image);
	free(pkl);
}

//=============================================================================
// pkl_dup
//=============================================================================
PKLExport PKLImage pkl_dup(PKLImage pkl)
{
	PKLImage dst;
	
	if(!pkl) return(NULL);
	
	dst = malloc(sizeof(struct _PKLImage));
	if(!dst) return(NULL);
	memset(dst, 0, sizeof(struct _PKLImage));
	
	dst->width = pkl->width;
	dst->height = pkl->height;
	dst->color = pkl->color;
	dst->channel = pkl->channel;
	dst->format = pkl->format;
	dst->compress = pkl->compress;
	
	if(pkl->image){
		dst->image = malloc(pkl->width*pkl->height*pkl->channel);
		if(!dst->image){
			free(dst);
			return(NULL);
		}
		memcpy(dst->image, pkl->image, pkl->width*pkl->height*pkl->channel);
	}
	return(dst);
}

//=============================================================================
// format_type
//=============================================================================
static int format_type(PKLImage pkl, FILE *image)
{
	unsigned char mark[PKL_HEADER_SIZE];
	int count, i;

	if( fread(mark, 1, PKL_HEADER_SIZE, image) < PKL_HEADER_SIZE ) return(1);

	count = sizeof(pklheader) /sizeof(struct PKL_HEADER);
	for(i=0; i<count; i++){
		if( !memcmp(mark, pklheader[i].mark, pklheader[i].size) ){
			pkl->format = pklheader[i].format;
			fseek(image, 0, SEEK_SET);
			return(0);
		}
	}
	return(1);
}

//=============================================================================
// pkl_format
//=============================================================================
PKLExport PKL_FORMAT pkl_format(PKLImage pkl)
{
	return(pkl->format);
}

//=================================================================================
// pkl_width
//=================================================================================
PKLExport int pkl_width(PKLImage pkl)
{
	return(pkl->width);
}

//=================================================================================
// pkl_height
//=================================================================================
PKLExport int pkl_height(PKLImage pkl)
{
	return(pkl->height);
}

//=============================================================================
// pkl_colorspace
//=============================================================================
PKLExport PKL_COLOR_SPACE pkl_colorspace(PKLImage pkl)
{
	return(pkl->color);
}

//=============================================================================
// pkl_compress
//=============================================================================
PKLExport int pkl_compress(PKLImage pkl, int level)
{
	if(level < 0 || level > 10) return(pkl->compress);
	pkl->compress = level;
	return(pkl->compress);
}

//=================================================================================
// pkl_save
//=================================================================================
PKLExport int pkl_save(PKLImage pkl, const char *out, PKL_FORMAT format)
{
	FILE *image;
	int result;
	
	image = fopen(out, "wb");
	if(!image) return(1);
	
	result = pkl_output(pkl, image, format);
	
	fclose(image);
	return(result);
}

//=================================================================================
// pkl_output
//=================================================================================
PKLExport int pkl_output(PKLImage pkl, FILE *out, PKL_FORMAT format)
{
	switch(format){
		case PKL_FORMAT_JPEG:
			return save_jpeg(pkl, out);
		case PKL_FORMAT_PNG:
			return save_png(pkl, out);
		case PKL_FORMAT_BITMAP:
			return save_bitmap(pkl, out);
		default:
			return(1);
	}
	return(1);
}

//=============================================================================
// pkl_count
//=============================================================================
PKLExport int pkl_count(PKLImage pkl)
{
	unsigned char *colors;
	int i, j, byte, bit, count=0, channel;
	union {
		int color;
		unsigned char row[PKL_CHANNEL];
	} pixel;
	
	switch(pkl->channel){
		case 1: channel=1; break;
		case 3:
		case 4: channel=3; break;
		default: return(0);
	}

	colors = malloc(256 * 256 * 32);
	memset(colors, 0, 256 * 256 * 32);

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			pixel.color=0;
			memcpy(pixel.row, &pkl->image[(i*pkl->width+j)*pkl->channel], channel);
			byte = pixel.color / 8;
			bit = pixel.color  % 8;
			
			if((colors[byte]&(1<<bit))) continue;
			colors[byte] = colors[byte]|(1<<bit);
			count++;
		}
	}
	free(colors);
	return(count);
}

//=================================================================================
// pkl_get_dpi
//=================================================================================
PKLExport double pkl_get_dpi(PKLImage pkl, PKL_RESOLUTION_TYPE res)
{
	switch(res){
		case PKL_RESOLUTION_HORZ:
			return(pkl->resh);
		case PKL_RESOLUTION_VERT:
			return(pkl->resv);
		default:
			;;
	}
	return(0.0);
}

//=================================================================================
// pkl_set_dpi
//=================================================================================
PKLExport int pkl_set_dpi(PKLImage pkl, PKL_RESOLUTION_TYPE res, double density)
{
	if(density < 0.0)
		return(1);
	
	switch(res){
		case PKL_RESOLUTION_HORZ:
			pkl->resh = density;
			break;
		case PKL_RESOLUTION_VERT:
			pkl->resv = density;
			break;
		case PKL_RESOLUTION_BOTH:
			pkl->resh = density;
			pkl->resv = density;
			break;
		default:
			return(1);
	}
	return(0);
}
