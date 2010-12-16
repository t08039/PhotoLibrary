#include "pikl_bitmap.h"

static void bmp_header(unsigned char *data, int offset, int value, int size);
static long convert_numeric(unsigned char *src, int size);
static int color_b2p(int biBitCount);
static double resolution_b2p(long r);
static int bmp_channel(int biBitCount);
static int bmp_width(int width, int channel);

//=============================================================================
// load_bitmap
//=============================================================================
int load_bitmap(PKLImage pkl, FILE *image)
{
	unsigned char header[BF_SIZE + BI_SIZE];
	unsigned char *p, stuck;
	int i, j, offset, scansize;
	
	if( fread(header, 1, sizeof(header), image) != sizeof(header) ) return(1);

	pkl->width   = convert_numeric(&header[18], 4);
	pkl->height  = convert_numeric(&header[22], 4);
	pkl->color   = color_b2p(convert_numeric(&header[28], 2));
	pkl->channel = bmp_channel(convert_numeric(&header[28], 2));
	pkl->resh    = resolution_b2p(convert_numeric(&header[38], 4));
	pkl->resv    = resolution_b2p(convert_numeric(&header[42], 4));
	if(pkl->color==PKL_UNKNOWN) return(1);
	
	pkl->image = malloc(pkl->width * pkl->height * pkl->channel);
	if(!pkl->image) return(1);
	
	offset = convert_numeric(&header[10], 4);
	scansize = bmp_width(pkl->width, pkl->channel);

	p = pkl->image + ((pkl->height-1) * pkl->width * pkl->channel);
	fseek(image, offset, SEEK_SET);
	
	for(i=0; i<pkl->height; i++){
		fread(p, 1, pkl->width*pkl->channel, image);
		for(j=0; j<pkl->width; j++){
			stuck = p[j*pkl->channel];
			p[j*pkl->channel] = p[j*pkl->channel+2];
			p[j*pkl->channel+2] = stuck;
		}
		p -= pkl->width*pkl->channel;
		offset += scansize;
		fseek(image, offset, SEEK_SET);
	}
	
	return(0);
}

//=================================================================================
// resolution_b2p
//=================================================================================
static double resolution_b2p(long r)
{
	return r==0 ? PKL_RESOLUTION : ((r * 254.0) / 10000.0);
}

//=============================================================================
// color_b2p
//	biBitCount = 1,4,8,16,24,32
//=============================================================================
static int color_b2p(int biBitCount)
{
	switch(biBitCount){
		case 24:
			return PKL_RGB;
		default:
			return PKL_UNKNOWN;
	}
	return PKL_UNKNOWN;
}

//=============================================================================
// bmp_channel
//	biBitCount = 1,4,8,16,24,32
//=============================================================================
static int bmp_channel(int biBitCount)
{
	switch(biBitCount){
		case 24:
			return 3;
		default:
			return 0;
	}
	return 0;
}

//=============================================================================
// convert_nv
//=============================================================================
static long convert_numeric(unsigned char *src, int size)
{
	int i;
	long dst=0;
	
	for(i=0; i<size; i++){
		dst += (src[i]<<(8*i));
	}
	return(dst);
}

//=============================================================================
// bmp_width
//=============================================================================
static int bmp_width(int width, int channel)
{
	int length;
	if( width*channel % 4 ){
		length = width*channel + (4 - width*channel % 4);
	}else{
		length = width*channel;
	}
	return(length);
}

//=============================================================================
// save_bitmap
//=============================================================================
int save_bitmap(PKLImage pkl, FILE *image)
{
	unsigned char data[BF_SIZE+BI_SIZE], *wrk, stuck;
	int linesize, i, j;
	
	if(pkl->color!=PKL_RGB) return(1);
	
	linesize = bmp_width(pkl->width, pkl->channel);
	
	data[0] = 'B';
	data[1] = 'M';
	bmp_header(data, 2, BF_SIZE + BI_SIZE + linesize*pkl->height, 4);
	bmp_header(data, 6, 0, 4);
	bmp_header(data, 10, BF_SIZE + BI_SIZE, 4);
	bmp_header(data, 14, BI_SIZE, 4);
	bmp_header(data, 18, pkl->width, 4);
	bmp_header(data, 22, pkl->height, 4);
	bmp_header(data, 26, 1, 2);
	bmp_header(data, 28, 24, 2);
	bmp_header(data, 30, 0, 24);
	bmp_header(data, 38, (long)((pkl->resh * 10000.0)/254.0+0.5), 4);
	bmp_header(data, 42, (long)((pkl->resv * 10000.0)/254.0+0.5), 4);
	fwrite(data, 1, BF_SIZE+BI_SIZE, image);
	
	wrk = malloc(linesize);
	for(i=pkl->height-1; i>=0; i--){
		memcpy(wrk, &pkl->image[i*pkl->width*pkl->channel], pkl->width*pkl->channel);
		for(j=0; j<pkl->width; j++){
			stuck = wrk[j*pkl->channel];
			wrk[j*pkl->channel] = wrk[j*pkl->channel+2];
			wrk[j*pkl->channel+2] = stuck;
		}
		fwrite(wrk, 1, linesize, image);
	}
	free(wrk);
	return(0);
}

//=============================================================================
// bmp_header
//=============================================================================
static void bmp_header(unsigned char *data, int offset, int value, int size)
{
	int i;
	for(i=0; i<size; i++){
		data[offset+i] = (unsigned char)(value >> (8*i));
	}
}
