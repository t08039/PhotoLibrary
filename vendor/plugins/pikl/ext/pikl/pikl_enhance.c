#include "pikl_enhance.h"

//=================================================================================
// pkl_unsharp
//=================================================================================
PKLExport int pkl_unsharp(PKLImage pkl, int threshold, double edge)
{
	int i, j, p, q, k;
	int cnt, value, stock[PKL_CHANNEL];
	unsigned char *wrk, *s;
	
	wrk = malloc(pkl->height * pkl->width * pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->height * pkl->width * pkl->channel);
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			cnt = 0;
			memset(stock, 0, sizeof(stock));
			
			for(p=-RADIUS; p<=RADIUS; p++){
				for(q=-RADIUS; q<=RADIUS; q++){
					if(j+q>=0 && j+q<pkl->width && i+p>=0 && i+p<pkl->height){
						for(k=0; k<pkl->channel; k++) stock[k] += pkl->image[((i+p)*pkl->width+(j+q))*pkl->channel+k];
						cnt++;
					}
				}
			}
			s = &wrk[(i * pkl->width + j) * pkl->channel];
			for(k=0; k<pkl->channel; k++)
				*s++ = stock[k] / cnt;
		}
	}
	
	for(i=0; i<pkl->height*pkl->width*pkl->channel; i++){
		value = abs(pkl->image[i]-wrk[i])<threshold ? pkl->image[i] : pkl->image[i]+(pkl->image[i]-wrk[i])*edge;
		pkl->image[i] = PKL_COLOR_CLIP(value);
	}
	free(wrk);
	return(0);
}

//=================================================================================
// pkl_contrast
//=================================================================================
PKLExport int pkl_contrast(PKLImage pkl, int rate)
{
	unsigned char ct[PKL_COLOR];
	int value, i;

	if(rate < -127 || rate > 127) return(1);

	if(rate < 0.0){
		//重みが負数の時は直線的に平坦化
		for(i=0; i<PKL_COLOR; i++){
			value = i + (i-127) * rate/127;
			ct[i] = PKL_COLOR_CLIP(value);
		}
	}else{
		//重みが正数の時は周波数
		for(i=0; i<PKL_COLOR; i++){
			value = i - rate*rate/127 * sin(M_PI/2+M_PI*i/255);
			ct[i] = PKL_COLOR_CLIP(value);
		}
	}

	for(i=0; i<pkl->width*pkl->height*pkl->channel; i++)
		pkl->image[i] = ct[pkl->image[i]];

	return(0);
}

//=================================================================================
// pkl_level def=0.0050
//=================================================================================
PKLExport int pkl_level(PKLImage pkl, double low, double high, double coeff)
{
	PKL_HISTGRAM hst[PKL_CHANNEL];
	int i, j, cnt, value;
	double lt, ht;
	
	if((low  < 0.0 && low  > 100.0) ||
	   (high < 0.0 && high > 100.0) ||
	   (coeff< 0.0 && coeff> 2.0)) return(1);
	
	memset(&hst, 0, sizeof(hst));
	for(i=0; i<pkl->height*pkl->width; i++){
		for(j=0; j<pkl->channel; j++){
			hst[j].data[ pkl->image[i*pkl->channel+j] ]++;
		}
	}

	lt = (low / 100.0) * pkl->height * pkl->width;
	ht = (high / 100.0) * pkl->height * pkl->width;
	
	for(i=0; i<pkl->channel; i++){
		cnt=0;
		for(j=0; j<PKL_COLOR; j++){
			cnt += hst[i].data[j];
			if(cnt > lt){
				hst[i].min = j;
				break;
			}
		}
		cnt=0;
		for(j=PKL_COLOR-1; j>=0; j--){
			cnt += hst[i].data[j];
			if(cnt > ht){
				hst[i].max = j;
				break;
			}
		}
	}

	//ヒストグラムからテーブルを生成
	for(i=0; i<pkl->channel; i++){
		for(j=0; j<PKL_COLOR; j++){
			value = ((coeff * j)-hst[i].min) / (hst[i].max-hst[i].min+1) * 255;
			hst[i].data[j] = PKL_COLOR_CLIP(value);
		}
	}

	for(i=0; i<pkl->width*pkl->height; i++)
		for(j=0; j<pkl->channel; j++)
			pkl->image[i*pkl->channel+j] = hst[j].data[pkl->image[i*pkl->channel+j]];

	return(0);
}

//=============================================================================
// pkl_brightness
//=============================================================================
PKLExport int pkl_brightness(PKLImage pkl, int color)
{
	int i;
	for(i=0; i<pkl->width*pkl->height*pkl->channel; i++)
		pkl->image[i] = PKL_COLOR_CLIP(pkl->image[i] + color);
	return(0);
}

//=============================================================================
// pkl_hls
//=============================================================================
PKLExport int pkl_hls(PKLImage pkl, double ym, double sm, double hd)
{
	int i;
	double c1, c2;	//色信号
	double y, s, h;	//y=輝度, s=彩度, h=色相
	
	if(pkl->color != PKL_RGB) return(1);
	
	for(i=0; i<pkl->width*pkl->height; i++){
		/* 輝度,色信号1,2 */
		y  = 0.299 * pkl->image[i*pkl->channel]   +
			 0.587 * pkl->image[i*pkl->channel+1] +
			 0.114 * pkl->image[i*pkl->channel+2];
		c1 = pkl->image[i*pkl->channel]   - y;
		c2 = pkl->image[i*pkl->channel+2] - y;
		
		/* 彩度 */
		s = sqrt(c1*c1 + c2*c2);
		
		/* 色相 */
		if(s < 0.0) h = 0.0;
		else        h = atan2(c1, c2) * 180.0 / M_PI;
		
		/* 調整を加える */
		y *= (1 + ym);
		s *= (1 + sm);
		h += hd;
		
		/* 調整値からRGBに戻す */
		c1 = s * sin(M_PI * h / 180.0);
		c2 = s * cos(M_PI * h / 180.0);

		pkl->image[i*pkl->channel]   = PKL_COLOR_CLIP(c1 + y);
		pkl->image[i*pkl->channel+1] = PKL_COLOR_CLIP(y - (0.299*c1 + 0.114*c2)/0.587);
		pkl->image[i*pkl->channel+2] = PKL_COLOR_CLIP(c2 + y);
	}
	return(0);
}

//=================================================================================
// pkl_gamma
//=================================================================================
PKLExport int pkl_gamma(PKLImage pkl, double gm)
{
	unsigned char gt[PKL_COLOR];
	int i;
	
	if(gm < 0.0) return(1);
	
	for(i=0; i<PKL_COLOR; i++)
		gt[i] = PKL_COLOR_CLIP(255.0 * pow(i/255.0, 1.0/gm));
	
	for(i=0; i<pkl->width*pkl->height*pkl->channel; i++)
		pkl->image[i] = gt[pkl->image[i]];

	return(0);
}

//=============================================================================
// pkl_noisecut(median filter)
//=============================================================================
PKLExport int pkl_noisecut(PKLImage pkl)
{
	unsigned char *wrk, *p, sort[PKL_CHANNEL][9], median[PKL_CHANNEL];
	int i, j, tx, ty, k, clipx, clipy, rd, s, t, n;

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			rd=0;
			for(ty=-1; ty<=1; ty++){
				for(tx=-1; tx<=1; tx++){
					clipx = j+tx < 0 ? 0 : j+tx>=pkl->width ? pkl->width-1 : j+tx;
					clipy = i+ty < 0 ? 0 : i+ty>=pkl->height ? pkl->height-1 : i+ty;
					p = &pkl->image[(clipy*pkl->width+clipx)*pkl->channel];
					for(k=0; k<pkl->channel; k++)
						sort[k][rd] = *p++;
					rd++;
				}
			}
			//中央値探索
			for(k=0; k<pkl->channel; k++){
				for(s=0; s<5; s++){
					for(t=1,n=0,median[k]=sort[k][0]; t<9; t++){
						if(median[k]<sort[k][t]){
							median[k]=sort[k][t];
							n=t;
						}
					}
					sort[k][n]=0;
				}
			}
			p= &wrk[(i*pkl->width+j)*pkl->channel];
			for(k=0; k<pkl->channel; k++) *p++=median[k];
		}
	}

	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=================================================================================
// pkl_trim
//=================================================================================
PKLExport int pkl_trim(PKLImage pkl, int sx, int sy, int width, int height)
{
	unsigned char *wrk;
	int p, y;
	
	if( sx < 0 || sy < 0 || width <= 0 || height <= 0 ) return(1);
	if( sx+width  > pkl->width  ) return(1);
	if( sy+height > pkl->height ) return(1);
	
	wrk = malloc(height * width * pkl->channel);
	if(!wrk) return(1);
	
	for(p=0,y=sy; y<sy+height; p++,y++){
		memcpy(&wrk[p*width*pkl->channel], &pkl->image[(y*pkl->width+sx)*pkl->channel], width*pkl->channel);
	}
	
	free(pkl->image);
	pkl->image  = wrk;
	pkl->height = height;
	pkl->width  = width;

	return(0);
}
