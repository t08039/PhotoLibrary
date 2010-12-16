#include "pikl_camera.h"

//=============================================================================
// pkl_vignette
//=============================================================================
PKLExport int pkl_vignette(PKLImage pkl, PKLColor color, int trans, int radius, int x, int y)
{
	double t, t1, tr, r, rr, a, w;
	unsigned char *p, at[PKL_CHANNEL][PKL_COLOR];
	int i, j, k, cx, cy;;
	
	if(trans<0 || trans>100) return(1);
	if(!color) return(1);

	t = (double)trans/100.0;
	t1 = 1.0 - t;

	for(i=0; i<pkl->channel; i++){
		w = (double)color->color[i]/255.0;
		for(j=0; j<PKL_COLOR; j++)
			at[i][j] = PKL_COLOR_CLIP((double)j * (t+t1 * w));
	}

	cx = (x<0 || x>=pkl->width) ? pkl->width/2 : x;
	cy = (y<0 || y>=pkl->height) ? pkl->height/2 : y;
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			r = sqrt((cx-j)*(cx-j) + (cy-i)*(cy-i));
			p = &pkl->image[(i*pkl->width+j)*pkl->channel];
			if(r > radius){
				for(k=0; k<pkl->channel; k++)
					*(p+k) = at[k][*(p+k)];
			}else{
				rr = r/(double)radius;
				tr = 1.2 - 1.2 * rr * rr;
				a = 1.0 < tr ? 1.0 : tr;
				
				for(k=0; k<pkl->channel; k++)
					*(p+k) = PKL_COLOR_CLIP((1.0-a) * at[k][*(p+k)] + a * *(p+k));
			}
		}
	}
	return(0);
}

////=============================================================================
//// pkl_spotlight
////=============================================================================
//PKLExport int pkl_spotlight(PKLImage pkl, int radius, PKLColor color, int trans)
//{
//	int i, j, k;
//	double rr, r, f, cx, cy, tr;
//
//	if(radius < 0) return(1);
//	
//	tr = (double)trans/100.0;
//	rr = radius * radius;
//	cx = (double)pkl->width/2.0;
//	cy = (double)pkl->height/2.0;
//	
//	for(i=0; i<pkl->height; i++){
//		for(j=0; j<pkl->width; j++){
//			r = (j-cx)*(j-cx) + (i-cy)*(i-cy);
//			f = 1.2 * (1.0 - r / rr);
//			f = f > 1.0 ? 1.0 : f;
//			
//			if(f > 0.0){
//				for(k=0; k<pkl->channel; k++)
//					pkl->image[(i*pkl->width+j)*pkl->channel+k]
//						= PKL_COLOR_CLIP(pkl->image[(i*pkl->width+j)*pkl->channel+k]*tr*f + color->color[k]*(1.0-tr*f));
//			}else{
//				memcpy(&pkl->image[(i*pkl->width+j)*pkl->channel], color, pkl->channel);
//			}
//		}
//	}
//	return(0);
//}

//=================================================================================
// pkl_vaseline
//=================================================================================
PKLExport int pkl_vaseline(PKLImage pkl, int radius, int x, int y)
{
	int i, j, s, t, k, z, cnt, stock[PKL_CHANNEL], cx, cy;
	unsigned char *wrk;
	double r;
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memcpy(wrk, pkl->image, pkl->width*pkl->height*pkl->channel);
	
	cx = (x<0 || x>=pkl->width) ? pkl->width/2 : x;
	cy = (y<0 || y>=pkl->height) ? pkl->height/2 : y;
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			r = sqrt((j-cx)*(j-cx)+(i-cy)*(i-cy));
			z = (int)((r-radius)*0.2);
			z = z>10 ? 10 : z;

			cnt = 0;
			memset(stock, 0, sizeof(stock));
			
			for(s=i-z; s<=i+z; s++){
				for(t=j-z; t<=j+z; t++){
					if(s<0 || s>=pkl->height || t<0 || t>=pkl->width) continue;
					for(k=0; k<pkl->channel; k++)
						stock[k] += pkl->image[(s*pkl->width+t)*pkl->channel+k];
					cnt++;
				}
			}
			if(cnt){
				for(k=0; k<pkl->channel; k++)
					wrk[(i*pkl->width+j)*pkl->channel+k] = PKL_COLOR_CLIP(stock[k]/cnt);
			}
		}
	}
	free(pkl->image);
	pkl->image= wrk;
	return(0);
}

//=============================================================================
// pkl_vv
//=============================================================================
PKLExport int pkl_vv(PKLImage pkl, PKLColor color, int trans, int radius, int x, int y)
{
	double t, t1, tr, r, rr, a, w;
	unsigned char at[PKL_CHANNEL][PKL_COLOR], *wrk;
	int i, j, k, p, q, offset, cnt, stock[PKL_CHANNEL], cx, cy;
	
	if(trans<0 || trans>100) return(1);
	if(!color) return(1);

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);

	t = (double)trans/100.0;
	t1 = 1.0 - t;

	for(i=0; i<pkl->channel; i++){
		w = (double)color->color[i]/255.0;
		for(j=0; j<PKL_COLOR; j++)
			at[i][j] = PKL_COLOR_CLIP((double)j * (t+t1 * w));
	}

	cx = (x<0 || x>=pkl->width) ? pkl->width/2 : x;
	cy = (y<0 || y>=pkl->height) ? pkl->height/2 : y;
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			r = sqrt((cx-j)*(cx-j) + (cy-i)*(cy-i));
			if(r > radius){
				offset = (int)((r-radius)*0.2);	//0.2‚ð•Ï‚¦‚é‚±‚Æ‚ÅA‚Ú‚©‚µ‹ï‡‚ª•Ï‚í‚é
				offset = offset>10 ? 10 : offset;
				cnt = 0;
				memset(stock, 0, sizeof(stock));
				
				for(p=i-offset; p<=i+offset; p++){
					for(q=j-offset; q<=j+offset; q++){
						if(p<0 || p>=pkl->height || q<0 || q>=pkl->width) continue;
						for(k=0; k<pkl->channel; k++)
							stock[k] += pkl->image[(p*pkl->width+q)*pkl->channel+k];
						cnt++;
					}
				}
				
				if(cnt==0) continue;
				for(k=0; k<pkl->channel; k++)
					wrk[(i*pkl->width+j)*pkl->channel+k] = at[k][PKL_COLOR_CLIP(stock[k]/cnt)];
			}else{
				rr = r/(double)radius;
				tr = 1.2 - 1.2 * rr * rr;
				a = 1.0 < tr ? 1.0 : tr;
				for(k=0; k<pkl->channel; k++)
					wrk[(i*pkl->width+j)*pkl->channel+k] = 
						PKL_COLOR_CLIP((1.0-a) * at[k][pkl->image[(i*pkl->width+j)*pkl->channel+k]] +
						a * pkl->image[(i*pkl->width+j)*pkl->channel+k]);
			}
		}
	}
	
	free(pkl->image);
	pkl->image=wrk;
	return(0);
}

//=============================================================================
// pkl_film
//=============================================================================
PKLExport int pkl_film(PKLImage pkl, PKLColor color, int trans)
{
	double t, t1, w;
	unsigned char at[PKL_CHANNEL][PKL_COLOR];
	int i, j, k;
	
	if(trans<0 || trans>100) return(1);
	if(!color) return(1);

	t = (double)trans/100.0;
	t1 = 1.0 - t;

	for(i=0; i<pkl->channel; i++){
		w = (double)color->color[i]/255.0;
		for(j=0; j<PKL_COLOR; j++)
			at[i][j] = PKL_COLOR_CLIP((double)j * (t+t1 * w));
	}
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			for(k=0; k<pkl->channel; k++)
				pkl->image[(i*pkl->width+j)*pkl->channel+k] = at[k][pkl->image[(i*pkl->width+j)*pkl->channel+k]];
		}
	}
	return(0);
}

