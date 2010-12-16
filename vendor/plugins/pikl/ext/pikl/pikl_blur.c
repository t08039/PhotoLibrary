#include "pikl_blur.h"

static int pkl_raw_blur(PKLImage pkl, int x, int y, double ef, int weight, double angle, double adr);

//=============================================================================
// pkl_blur
//=============================================================================
PKLExport int pkl_blur(PKLImage pkl, int weight)
{
	int i, j, p, q, k;
	int cnt, stock[PKL_CHANNEL];
	unsigned char *wrk;
	
	if(weight<=0)
		return(0);
	
	wrk = malloc(pkl->height * pkl->width * pkl->channel);
	if(!wrk) return(1);
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			cnt = 0;
			memset(stock, 0, sizeof(stock));
			
			for(p=-weight; p<=weight; p++){
				for(q=-weight; q<=weight; q++){
					if(j+q<0 || j+q>=pkl->width || i+p<0 || i+p>=pkl->height) continue;
					for(k=0; k<pkl->channel; k++)
						stock[k] += pkl->image[((i+p)*pkl->width+(j+q))*pkl->channel+k];
					cnt++;
				}
			}
			if(cnt==0) continue;
			for(k=0; k<pkl->channel; k++)
				wrk[(i * pkl->width + j) * pkl->channel+k] = PKL_COLOR_CLIP(stock[k]/cnt);
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_gaussblur
//=============================================================================
PKLExport int pkl_gaussblur(PKLImage pkl, double weight)
{
	int range, i, j, s, t, p;
	double sg, *gauss, value[PKL_CHANNEL];
	unsigned char *wrk;
	
	if(weight<=0.0) return(1);
	
	wrk = malloc(pkl->height * pkl->width * pkl->channel);
	if(!wrk) return(1);

	range = weight * 3;
	gauss = malloc(sizeof(double)*(range+1));
	memset(gauss, 0, sizeof(gauss));
	
	//gaussian table
	for(i=0; i<=range; i++)
		gauss[i] = exp( -1.0*i*i / (2*weight*weight));
	
	//horizon(pkl-->wrk)
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			memset(value, 0, sizeof(double)*PKL_CHANNEL);
			sg=0;
			for(s=-range; s<=range; s++){
				t = j+s<0 ? 0 : j+s>=pkl->width ? pkl->width-1 : j+s;
				sg += gauss[abs(s)];
				for(p=0; p<pkl->channel; p++){
					value[p] += pkl->image[(i*pkl->width+t)*pkl->channel+p] * gauss[abs(s)];
				}
			}
			for(p=0; p<pkl->channel; p++)
				wrk[(i*pkl->width+j)*pkl->channel+p] = PKL_COLOR_CLIP(value[p]/sg);
		}
	}
	
	//vertical(wrk-->pkl)
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			memset(value, 0, sizeof(double)*PKL_CHANNEL);
			sg=0;
			for(s=-range; s<=range; s++){
				t = i+s<0 ? 0 : i+s>=pkl->height ? pkl->height-1 : i+s;
				sg += gauss[abs(s)];
				for(p=0; p<pkl->channel; p++){
					value[p] += wrk[(t*pkl->width+j)*pkl->channel+p] * gauss[abs(s)];
				}
			}
			for(p=0; p<pkl->channel; p++)
				pkl->image[(i*pkl->width+j)*pkl->channel+p] = PKL_COLOR_CLIP(value[p]/sg);
		}
	}
	
	free(gauss);
	free(wrk);
	return(0);
}

//=============================================================================
// pkl_rblur(放射)
//=============================================================================
PKLExport int pkl_rblur(PKLImage pkl, int x, int y, double ef, int weight)
{
	return pkl_raw_blur(pkl, x, y, ef, weight, 0.0, 0.0);
}

//=============================================================================
// pkl_ablur(回転)
//=============================================================================
PKLExport int pkl_ablur(PKLImage pkl, int x, int y, double ef, int weight)
{
	return pkl_raw_blur(pkl, x, y, ef, weight, 0.0, M_PI/2.0);
}

//=============================================================================
// pkl_wblur
//=============================================================================
PKLExport int pkl_wblur(PKLImage pkl, int x, int y, double ef, int weight, double angle)
{
	return pkl_raw_blur(pkl, x, y, ef, weight, angle, M_PI*angle/180.0);
}

//=============================================================================
// pkl_rab_blur(放射,回転,うずまきのコア)
//=============================================================================
static int pkl_raw_blur(PKLImage pkl, int x, int y, double ef, int weight, double angle, double adr)
{
	unsigned char *wrk;
	int i, j, k, t, tx, ty, pat;
	long color[PKL_CHANNEL], range;
	double rad, dis, disI, rate, disMAX;
	double ox, oy, dx, dy, nf;

	if(ef==0.0 || weight==0) return(1);

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);

	nf = weight/2;
	//ox = (double)(pkl->width)/2.0;
	//oy = (double)(pkl->height)/2.0;
	ox = (x<0 || x>=pkl->width) ? (double)(pkl->width)/2.0 : (double)x;			//中心点を決める
	oy = (y<0 || y>=pkl->height) ? (double)(pkl->height)/2.0 : (double)y;		//中心点を決める
	dx = (double)pkl->width-ox;
	dy = (double)pkl->height-oy;
	disMAX = sqrt(dx*dx+dy*dy);

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			range=0;
			memset(color, 0, sizeof(color));
			
			dx = (double)j-ox;
			dy = (double)i-oy;
			//rad = ((dx!=0.0) ? atan(dy/dx) : M_PI/2.0) + (M_PI*angle/180.0);	//うずまき状
			//rad = ((dx!=0.0) ? atan(dy/dx) : M_PI/2.0);						//放射状
			//rad = ((dx!=0.0) ? atan(dy/dx) : M_PI/2.0) + (M_PI/2.0);			//回転
			rad = ((dx!=0.0) ? atan(dy/dx) : M_PI/2.0) + adr;
			dis = sqrt(dx*dx+dy*dy);
			rate = (ef*dis/disMAX) / (double)nf;

			for(t=0; t<weight; t++){
				pat = (t==nf) ? 3 : 1;
				disI = (double)(t-nf)*rate;
				tx = (int)(disI * cos(rad)) + j;
				ty = (int)(disI * sin(rad)) + i;
				tx = tx<0 ? 0 : tx>=pkl->width ? pkl->width-1 : tx;
				ty = ty<0 ? 0 : ty>=pkl->height ? pkl->height-1 : ty;
				for(k=0; k<pkl->channel; k++)
					color[k] += pkl->image[(ty*pkl->width+tx)*pkl->channel+k] * pat;
				range += pat;
			}
			
			for(k=0; k<pkl->channel; k++)
				wrk[(i*pkl->width+j)*pkl->channel+k] = PKL_COLOR_CLIP(color[k]/range);
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_mblur
//=============================================================================
PKLExport int pkl_mblur(PKLImage pkl, double angle, int weight)
{
	unsigned char *wrk;
	int i, j, k, t, tx, ty;
	int *dx, *dy, color[PKL_CHANNEL];
	double s, c;
	
	if(weight < 2) return(0);
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);
	
	//任意角度のズレ量テーブルを作成
	s = sin((angle+180.0) * M_PI / 180.0);
	c = cos((angle+180.0) * M_PI / 180.0);
	
	dx = malloc(sizeof(int)*(weight+1));
	dy = malloc(sizeof(int)*(weight+1));
	memset(dx, 0, sizeof(int)*(weight+1));
	memset(dy, 0, sizeof(int)*(weight+1));
	
	for(i=1; i<=weight; i++){
		dx[i] = (int)(c * (i+1) + 0.5);
		dy[i] = (int)(s * (i+1) + 0.5);
	}
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			memset(color, 0, sizeof(color));
			for(t=0; t<=weight; t++){
				tx = j+dx[t] < 0 ? 0 : j+dx[t]>=pkl->width ? pkl->width-1 : j+dx[t];
				ty = i+dy[t] < 0 ? 0 : i+dy[t]>=pkl->height ? pkl->height-1 : i+dy[t];
				
				for(k=0; k<pkl->channel; k++)
					color[k] += pkl->image[(ty*pkl->width+tx)*pkl->channel+k];
			}
			
			for(k=0; k<pkl->channel; k++)
				wrk[(i*pkl->width+j)*pkl->channel+k] = PKL_COLOR_CLIP(color[k]/weight);
		}
	}
	
	free(dx);
	free(dy);
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

