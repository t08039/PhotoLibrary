#include "pikl_resize.h"

static int resize_basic(PKLImage pkl, int width, int height, PKL_SAMPLE sample);
static int resize_pa(PKLImage pkl, int width, int height);
static int resize_lz(PKLImage pkl, int width, int height);
static int resize_lz_cr(PKLImage pkl, double scale, int ax);
static double lz_weight(double x);
static int get_pixel(PKLImage pkl, int x, int y, unsigned char *pix);

//=================================================================================
// pkl_resize
//=================================================================================
PKLExport int pkl_resize(PKLImage pkl, int width, int height, PKL_SAMPLE sample)
{
	// color mode check
	
	if(width < 1 || height < 1) return(1);
	if(width==pkl->width && height==pkl->height) return(0);
	
	switch(sample){
		case PKL_SAMPLE_NN:
		case PKL_SAMPLE_BL:
		case PKL_SAMPLE_BC:
			return resize_basic(pkl, width, height, sample);
		case PKL_SAMPLE_PA:
			if(width<pkl->width && height<pkl->height)
				return resize_pa(pkl, width, height);
		case PKL_SAMPLE_LZ:
			return resize_lz(pkl, width, height);
		default:
			return(1);
	}
	return(1);
}

//=================================================================================
// resize_basic
//=================================================================================
static int resize_basic(PKLImage pkl, int width, int height, PKL_SAMPLE sample)
{
	unsigned char *wrk, color[PKL_CHANNEL];
	double zx, zy, fx, fy;
	int i, j;
	void (*op)(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);

	if((op=sample_select(sample)) == NULL)
		return(1);
	
	wrk = malloc(width*height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, width*height*pkl->channel);
	memset(color, 0, PKL_CHANNEL);
	
	zx = (double)pkl->width / (double)width;
	zy = (double)pkl->height/ (double)height;

	for(i=0; i<height; i++){
		for(j=0; j<width; j++){
			fx = zx * j;
			fy = zy * i;
			op(pkl, &wrk[(i*width+j)*pkl->channel], fx, fy, color);
		}
	}
	free(pkl->image);
	pkl->image = wrk;
	pkl->width = width;
	pkl->height = height;
	return(0);
}

//=============================================================================
// resize_pa
//=============================================================================
static int resize_pa(PKLImage pkl, int width, int height)
{
	unsigned char *wrk;
	int i, j;
	double zx, zy;
	
	wrk = malloc(width * height * pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, width*height*pkl->channel);
	
	zx = (double)pkl->width  / (double)width;
	zy = (double)pkl->height / (double)height;

	for(i=0; i<height; i++){
		for(j=0; j<width; j++){
			double w1, w2, h1, h2;
			double w1b, w2b, h1b, h2b;
			double bi, ixg[PKL_CHANNEL] = {0.0, 0.0, 0.0, 0.0};
			int fsx ,fsy, s, t, k;
			
			w1b = modf(zx *  j,    &w1);
			w2b = modf(zx * (j+1), &w2);
			h1b = modf(zy *  i,    &h1);
			h2b = modf(zy * (i+1), &h2);

			for(s=(int)w1; s<=(int)w2; s++){
				for(t=(int)h1; t<=(int)h2; t++){
					bi = 1.0;
					if(s == (int)w1) bi*=(1-w1b);
					if(s == (int)w2) bi*=(w2b);
					if(t == (int)h1) bi*=(1-h1b);
					if(t == (int)h2) bi*=(h2b);
					fsx = (s < 0) ? 0 : (s > pkl->width -1) ? pkl->width -1 : s;
					fsy = (t < 0) ? 0 : (t > pkl->height-1) ? pkl->height-1 : t;
					for(k=0; k<pkl->channel; k++)
						ixg[k] += pkl->image[(fsy*pkl->width+fsx)*pkl->channel+k] * bi;
				}
			}
			for(k=0; k<pkl->channel; k++)
				wrk[(i*width+j)*pkl->channel+k] = PKL_COLOR_CLIP(ixg[k]/(zx*zy));
		}
	}
	free(pkl->image);
	pkl->image = wrk;
	pkl->width  = width;
	pkl->height = height;
	return(0);
}

//=============================================================================
// resize_lz
//=============================================================================
static int resize_lz(PKLImage pkl, int width, int height)
{
	double scale;
	
	scale = (double)width / (double)pkl->width;
	if( resize_lz_cr(pkl, scale, 1) ) return(1);
	
	scale = (double)height / (double)pkl->height;
	if( resize_lz_cr(pkl, scale, 0) ) return(1);
	
	return(0);
}

//=============================================================================
// resize_lz_cr
//=============================================================================
static int resize_lz_cr(PKLImage pkl, double scale, int ax)
{
	int hsize, vsize, i, j, k, l, lMax, zx, zy, factor;
	double *w0, *w1, T, t, F, data[PKL_CHANNEL];
	unsigned char *wrk, v[PKL_CHANNEL], v1[PKL_CHANNEL], v2[PKL_CHANNEL];
	int mx, Mx;

	if(ax){
		zy = hsize = (int)(0.5 + pkl->width * scale);
		zx = vsize = pkl->height;
	}else{
		zx = hsize = pkl->width;
		zy = vsize = (int)(0.5 + pkl->height * scale);
	}

	wrk = malloc(hsize*vsize*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, hsize*vsize*pkl->channel);

	factor = (scale>=1) ? 1 : (int)(1.4/scale); 
	lMax = factor << 1;
	w0 = malloc(lMax * sizeof(double));
	w1 = malloc(lMax * sizeof(double));

	for(j=0; j<zy; j++){
		F = modf((double)j/scale, &T);
		for(l=0; l<lMax; l++){
			w0[lMax-l-1] = lz_weight( ((lMax-l-1) + F)/factor );
			w1[l]        = lz_weight( ((l+1)      - F)/factor );
		}
		t = 0.0;
		for(l=0; l<lMax; l++){
			t+=w0[l];
			t+=w1[l];
		}
		t /= (double)factor;
		for(l=0; l<lMax; l++){
			w0[l] /= t;
			w1[l] /= t;
		}

		if(ax){
			for(i=0; i<zx; i++){
				memset(data, 0, sizeof(data));
				for(l=0; l<lMax; l++){
					mx = (T-lMax+l+1 < 0) ? 0 : T-lMax+l+1;
					Mx = (T+l+1 >= pkl->width) ? pkl->width-1 : T+l+1;
					get_pixel(pkl, Mx, i, v1);
					get_pixel(pkl, mx, i, v2);
					for(k=0; k<pkl->channel; k++) data[k]+=(w1[l]*v1[k]+w0[lMax-l-1]*v2[k]);
				}
				for(k=0; k<pkl->channel; k++) v[k]=PKL_MINMAX(0, 255, 0.5+(data[k]/factor));
				if(i>=0 && i<vsize && j>=0 && j<hsize) memcpy(&wrk[(i*hsize+j)*pkl->channel], &v, pkl->channel);
			}
		}else{
			for(i=0; i<zx; i++){
				memset(data, 0, sizeof(data));
				for(l=0; l<lMax; l++){
					mx = (T-lMax+l+1 < 0) ? 0 : T-lMax+l+1;
					Mx = (T+l+1 >= pkl->height) ? pkl->height-1 : T+l+1;
					get_pixel(pkl, i, Mx, v1);
					get_pixel(pkl, i, mx, v2);
					for(k=0; k<pkl->channel; k++) data[k]+=(w1[l]*v1[k]+w0[lMax-l-1]*v2[k]);
				}
				for(k=0; k<pkl->channel; k++) v[k]=PKL_MINMAX(0, 255, 0.5+(data[k]/factor));
				if(j>=0 && j<vsize && i>=0 && i<hsize) memcpy(&wrk[(j*hsize+i)*pkl->channel], &v, pkl->channel);
			}
		}
	}
	free(w0);
	free(w1);
	free(pkl->image);
	pkl->image  = wrk;
	pkl->width  = hsize;
	pkl->height = vsize;
	return(0);
}

//=============================================================================
// get_pixel
//=============================================================================
static int get_pixel(PKLImage pkl, int x, int y, unsigned char *pix)
{
	if(x<0 || x>=pkl->width || y<0 || y>=pkl->height) return(1);
	memset(pix, 0, pkl->channel);
	memcpy(pix, &pkl->image[(y*pkl->width+x)*pkl->channel], pkl->channel);
	return(0);
}

//=============================================================================
// lz_weight
//=============================================================================
static double lz_weight(double x)
{
	double PIx, PIx2;
	
	PIx  = M_PI * x;
	PIx2 = PIx / 2.0;

	if(x>=2.0 || x<=-2.0){
		return (0.0);
	}else
	if(x == 0.0){
		return (1.0);
	}else{
		return(sin(PIx) / PIx * sin(PIx2) / PIx2);
	}
}

