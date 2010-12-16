#include "pikl_effect.h"

//=============================================================================
// pkl_invert
//=============================================================================
PKLExport int pkl_invert(PKLImage pkl)
{
	int i;
	for(i=0; i<pkl->height*pkl->width*pkl->channel; i++)
		pkl->image[i] ^= 0xff;
	return(0);
}

//=================================================================================
// pkl_solarization
//=================================================================================
PKLExport int pkl_solarization(PKLImage pkl, int dark, unsigned char weight)
{
	int i;
	
	if(dark){
		for(i=0; i<pkl->height*pkl->width*pkl->channel; i++){
			if(pkl->image[i] <= weight)
				pkl->image[i] ^= 0xff;
		}
	}else{
		for(i=0; i<pkl->height*pkl->width*pkl->channel; i++){
			if(pkl->image[i] >= weight)
				pkl->image[i] ^= 0xff;
		}
	}
	
	return(0);
}

//=============================================================================
// pkl_sepia
//=============================================================================
PKLExport int pkl_sepia(PKLImage pkl, double red_weight, double green_weight, double blue_weight)
{
	unsigned char *wrk;
	int i, j, p;

	if(pkl->color!=PKL_RGB) return(1);

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			//color = ((pkl->image[(i*pkl->width+j)*pkl->channel+0]+
			//		  pkl->image[(i*pkl->width+j)*pkl->channel+1]+
			//		  pkl->image[(i*pkl->width+j)*pkl->channel+2]) / 3);
			//wrk[(i*pkl->width+j)*pkl->channel+0] = PKL_COLOR_CLIP(color*red_weight);
			//wrk[(i*pkl->width+j)*pkl->channel+1] = PKL_COLOR_CLIP(color*green_weight);
			//wrk[(i*pkl->width+j)*pkl->channel+2] = PKL_COLOR_CLIP(color*blue_weight);
			
			p = (i*pkl->width+j)*pkl->channel;
			wrk[p+0] = PKL_COLOR_CLIP(pkl->image[p+0]*red_weight);
			wrk[p+1] = PKL_COLOR_CLIP(pkl->image[p+1]*green_weight);
			wrk[p+2] = PKL_COLOR_CLIP(pkl->image[p+2]*blue_weight);
		}
	}

	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_oilpaint
//=============================================================================
PKLExport int pkl_oilpaint(PKLImage pkl, int weight)
{
	int color[PKL_COLOR], gray;
	int i, j, k, max, s, t, sx, sy, tx, ty;
	unsigned char *wrk;
	
	if(weight <= 0)
		return(0);

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);
	
	for(i=0; i<pkl->width*pkl->height*pkl->channel; i+=pkl->channel){
		gray=0;
		for(k=0; k<pkl->channel; k++)
			gray += pkl->image[i+k];
		wrk[i] = PKL_COLOR_CLIP(gray/pkl->channel);
	}

	//get histgram
	memset(color, 0, sizeof(int)*PKL_COLOR);
	for(i=0; i<pkl->height*pkl->width; i+=pkl->channel) color[wrk[i]]++;

	//周辺ピクセルから最頻色を選択
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			max = 0;
			tx=j;
			ty=i;
			for(s=0; s<weight; s++){
				for(t=0; t<weight; t++){
					sx = (t+j >= pkl->width) ? pkl->width-1 : t+j;
					sy = (s+i >= pkl->height) ? pkl->height-1 : s+i;
					if(max<color[wrk[(sy*pkl->width+sx)*pkl->channel]]){
						max = color[wrk[(sy*pkl->width+sx)*pkl->channel]];
						tx = sx;
						ty = sy;
					}
				}
			}
			//オリジナルの色を適用
			memcpy(&wrk[(i*pkl->width+j)*pkl->channel], &pkl->image[(ty*pkl->width+tx)*pkl->channel], pkl->channel);
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_noise
//=============================================================================
PKLExport int pkl_noise(PKLImage pkl, int noise)
{
	unsigned char *wrk;
	int i, j, k, sx, sy, nw, nh;
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);

	#ifdef WIN32
	srand(1);
	#else
	srand48(1);
	#endif
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			//nw = rand()%(noise*2+1)-noise;
			#ifdef WIN32
			nw = rand()%(noise*2+1)-noise;
			nh = rand()%(noise*2+1)-noise;
			#else
			nw = lrand48()%(noise*2+1)-noise;
			nh = lrand48()%(noise*2+1)-noise;
			#endif
			sx = j+nw<0 ? 0 : j+nw>=pkl->width ? pkl->width-1 : j+nw;
			sy = i+nh<0 ? 0 : i+nh>=pkl->height ? pkl->height-1 : i+nh;
			for(k=0; k<pkl->channel; k++)
				wrk[(i*pkl->width+j)*pkl->channel+k] = pkl->image[(sy*pkl->width+sx)*pkl->channel+k];
		}
	}

	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_vtr
//=============================================================================
PKLExport int pkl_vtr(PKLImage pkl, int colspace, int gst, int cst)
{
	unsigned char *wrk;
	int i, j, k, t;
	int h=0, l_det=0;

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);

	for(i=0; i<pkl->height; i++){
		if(l_det >= colspace){
			h = !h;
			l_det=0;
		}
		l_det++;
		
		for(j=0; j<pkl->width; j++){
			t = j+gst<0 ? 0 : j+gst>=pkl->width ? pkl->width-1 : j+gst;
			for(k=0; k<pkl->channel; k++)
				wrk[(i*pkl->width+j)*pkl->channel+k] =
					PKL_COLOR_CLIP((pkl->image[(i*pkl->width+j)*pkl->channel+k] +
									pkl->image[(i*pkl->width+t)*pkl->channel+k])/2 + (cst-127)*h);
		}
	}

	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_edgepaint
//=============================================================================
PKLExport int pkl_edgepaint(PKLImage pkl, int edge)
{
	unsigned char *wrk;
	int i, j, s, t, k, color, clipx, clipy, gray;
	const int filter[9] = { -1, -1, -1, 
							-1,  8, -1, 
							-1, -1, -1};
	
	wrk = malloc(pkl->width*pkl->height);
	if(!wrk) return(1);
	
	/* convert grayscale */
	for(i=0; i<pkl->width*pkl->height; i++){
		gray=0;
		for(k=0; k<pkl->channel; k++)
			gray += pkl->image[i*pkl->channel+k];
		wrk[i] = PKL_COLOR_CLIP(gray/pkl->channel);
	}

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			color=0;
			for(s=0; s<3; s++){
				for(t=0; t<3; t++){
					clipx = (j+t)>=pkl->width  ? pkl->width-1  : j+t;
					clipy = (i+s)>=pkl->height ? pkl->height-1 : i+s;
					color += wrk[clipy*pkl->width+clipx] * filter[t+s*3];
				}
			}
			if(color>0){
				//memset(&pkl->image[(i*pkl->width+j)*pkl->channel], 0, pkl->channel);
				for(s=0; s<edge; s++){
					for(t=0; t<edge; t++){
						clipx = (j+t)>=pkl->width  ? pkl->width-1  : j+t;
						clipy = (i+s)>=pkl->height ? pkl->height-1 : i+s;
						memset(&pkl->image[(clipy*pkl->width+clipx)*pkl->channel], 0, pkl->channel);
					}
				}
			}
		}
	}
	free(wrk);
	return(0);
}

//=============================================================================
// pkl_kuwahara
//=============================================================================
PKLExport int pkl_kuwahara(PKLImage pkl, int range)
{
	unsigned char *wrk, *p;
	int i, j, k, count, tr, t, tx, ty, clipx, clipy;
	int value[4], mincolor, idx, total[PKL_CHANNEL];
	struct { int sx, sy, ex, ey; } r[4];	//rect
	unsigned char max_color[PKL_CHANNEL], min_color[PKL_CHANNEL], c;
	
	if(range<=1) return(0);
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);

	//領域のピクセル数
	count = range*range;
	tr = range-1;
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			/* カレントピクセルを中心に4つの領域に分ける */
			//左上(0)
			r[0].sx = j-tr;
			r[0].sy = i-tr;
			r[0].ex = j;
			r[0].ey = i;
			//右上(1)
			r[1].sx = j;
			r[1].sy = i-tr;
			r[1].ex = j+tr;
			r[1].ey = i;
			//右下(2)
			r[2].sx = j;
			r[2].sy = i;
			r[2].ex = j+tr;
			r[2].ey = i+tr;
			//左下(3)
			r[3].sx = j-tr;
			r[3].sy = i;
			r[3].ex = j;
			r[3].ey = i+tr;
			
			/* 各領域で各色の最大値と最小値の差を累積する */
			memset(value, 0, sizeof(value));
			for(t=0; t<4; t++){
				memset(max_color, 0x00, PKL_CHANNEL);
				memset(min_color, 0xff, PKL_CHANNEL);
				for(ty=r[t].sy; ty<=r[t].ey; ty++){
					for(tx=r[t].sx; tx<=r[t].ex; tx++){
						clipx = tx<0 ? 0 : tx>=pkl->width ? pkl->width-1 : tx;
						clipy = ty<0 ? 0 : ty>=pkl->height ? pkl->height-1 : ty;
						p = &pkl->image[(clipy*pkl->width+clipx)*pkl->channel];
						for(k=0; k<pkl->channel; k++){
							c = *p++;
							max_color[k] = (c > max_color[k]) ? c : max_color[k];
							min_color[k] = (c < min_color[k]) ? c : min_color[k];
						}
					}
				}
				
				for(k=0; k<pkl->channel; k++)
					value[t] += (max_color[k] - min_color[k]);
			}
			
			/* 各領域で最も色の差分が小さい領域を選択する */
			mincolor = value[0];
			idx = 0;
			for(t=1; t<4; t++){
				if(value[t]<mincolor){
					mincolor = value[t];
					idx = t;
				}
			}
			
			/* 選択された領域の色の平均をピクセルに適用する */
			memset(total, 0, sizeof(total));
			for(ty=r[idx].sy; ty<=r[idx].ey; ty++){
				for(tx=r[idx].sx; tx<=r[idx].ex; tx++){
					clipx = tx<0 ? 0 : tx>=pkl->width ? pkl->width-1 : tx;
					clipy = ty<0 ? 0 : ty>=pkl->height ? pkl->height-1 : ty;
					p = &pkl->image[(clipy*pkl->width+clipx)*pkl->channel];
					for(k=0; k<pkl->channel; k++)
						total[k] += *p++;
				}
			}
			p =&wrk[(i*pkl->width+j)*pkl->channel];
			for(k=0; k<pkl->channel; k++)
				*p++ = PKL_COLOR_CLIP(total[k]/count);
		}
	}

	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_edge
//=============================================================================
PKLExport int pkl_edge(PKLImage pkl, int threshold)
{
	unsigned char *wrk, *p;
	int i, j, k, color[PKL_CHANNEL], total;
	int r1, r2, r3;
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0xff, pkl->width*pkl->height*pkl->channel);
	
	r3 = sqrt((double)pkl->channel);

	for(i=0; i<pkl->height; i++){
		r1=r2=0;
		for(j=0; j<pkl->width; j++){
			//x方向
			if(j+1 < pkl->width){
				p = &pkl->image[(i*pkl->width+j)*pkl->channel];
				for(k=0; k<pkl->channel; k++){
					color[k] = *p - *(p+pkl->channel);
					color[k] *= color[k];
					p++;
				}
				total=0;
				for(k=0; k<pkl->channel; k++)
					total += color[k];
				r1 = sqrt(total)/r3;
			}
			
			//y方向
			if(i+1 < pkl->height){
				p = &pkl->image[(i*pkl->width+j)*pkl->channel];
				for(k=0; k<pkl->channel; k++){
					color[k] = *p - *(p+pkl->width*pkl->channel);
					color[k] *= color[k];
					p++;
				}
				total=0;
				for(k=0; k<pkl->channel; k++)
					total += color[k];
				r2 = sqrt(total)/r3;
			}

			p = &wrk[(i*pkl->width+j)*pkl->channel];
			if(r1>=threshold || r2>=threshold)
				for(k=0; k<pkl->channel; k++) *p++=0;
		}
	}
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_dither
//=============================================================================
PKLExport int pkl_dither(PKLImage pkl, PKL_DITHER dither)
{
	double FloydSteinberg[2][3] = {
		{       -1,       -1, 7.0/16.0},
		{ 3.0/16.0, 5.0/16.0, 1.0/16.0} };
	
	double Stucci[3][5] = {
		{      -1,       -1,       -1, 8.0/42.0, 4.0/42.0},
		{2.0/42.0, 4.0/42.0, 8.0/42.0, 4.0/42.0, 2.0/42.0},
		{1.0/42.0, 2.0/42.0, 4.0/42.0, 2.0/42.0, 1.0/42.0} };

	double Sierra[3][5] = {
		{      -1,       -1,       -1, 5.0/32.0, 3.0/32.0},
		{2.0/32.0, 4.0/32.0, 5.0/32.0, 4.0/32.0, 2.0/32.0},
		{      -1, 2.0/32.0, 3.0/32.0, 2.0/32.0,       -1} };

	double JaJuNi[3][5] = {
		{      -1,       -1,       -1, 7.0/48.0, 5.0/48.0},
		{3.0/48.0, 5.0/48.0, 7.0/48.0, 5.0/48.0, 3.0/48.0},
		{1.0/48.0, 3.0/48.0, 5.0/48.0, 3.0/48.0, 1.0/48.0} };

	unsigned char *wrk, pix;
	double *pt, err;
	int i, j, k, gray, s, t, tx, ty, row, col, xrange;
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);

	//ディザパタンを選択
	switch(dither){
		case PKL_DITHER_STUCCI:
			row = 5;
			col = 3;
			pt = &(Stucci[0][0]);
			break;
			
		case PKL_DITHER_SIERRA:
			row = 5;
			col = 3;
			pt = &(Sierra[0][0]);
			break;
			
		case PKL_DITHER_JAJUNI:
			row = 5;
			col = 3;
			pt = &(JaJuNi[0][0]);
			break;
			
		case PKL_DITHER_FLOYDSTEINBERG:
		default:
			row = 3;
			col = 2;
			pt = &(FloydSteinberg[0][0]);
	}

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			gray=0;
			for(k=0; k<pkl->channel; k++)
				gray += pkl->image[(i*pkl->width+j)*pkl->channel+k];
			pkl->image[(i*pkl->width+j)*pkl->channel] = PKL_COLOR_CLIP(gray/pkl->channel);
		}
	}

	xrange = (row-1)/2;

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			pix = pkl->image[(i*pkl->width+j)*pkl->channel];
			if(pix>127)
				memset(&wrk[(i*pkl->width+j)*pkl->channel], 0xff, pkl->channel);
			else
				memset(&wrk[(i*pkl->width+j)*pkl->channel], 0x00, pkl->channel);
			err = pix>127 ? pix-255 : pix;

			for(s=0; s<col; s++){
				for(t=-xrange; t<=xrange; t++){
					tx = j+t;
					ty = i+s;
					if(tx<0 || tx>=pkl->width || ty>=pkl->height) continue;
					if (pt[s*row+t+xrange] < 0) continue;
					
					pkl->image[(ty*pkl->width+tx)*pkl->channel] =
						PKL_COLOR_CLIP(pkl->image[(ty*pkl->width+tx)*pkl->channel] + err * pt[s*row+t+xrange]);
				}
			}
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_colordither
//=============================================================================
PKLExport int pkl_colordither(PKLImage pkl, int weight)
{
	double FloydSteinberg[2][3] = {
		{       -1,       -1, 7.0/16.0},
		{ 3.0/16.0, 5.0/16.0, 1.0/16.0} };
	unsigned char *wrk, pix, *graypic;
	double *pt, err;
	int i, j, k, gray, s, t, tx, ty, row, col, xrange;
	
	row = 3;
	col = 2;
	pt = &(FloydSteinberg[0][0]);
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0xff, pkl->width*pkl->height*pkl->channel);

	graypic = malloc(pkl->width*pkl->height);
	if(!graypic) return(1);

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			gray=0;
			for(k=0; k<pkl->channel; k++)
				gray += pkl->image[(i*pkl->width+j)*pkl->channel+k];
			graypic[i*pkl->width+j] = PKL_COLOR_CLIP(gray/pkl->channel);
		}
	}
	xrange = (row-1)/2;

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			pix = graypic[i*pkl->width+j];
			if(pix<127){
				memcpy(&wrk[(i*pkl->width+j)*pkl->channel], &pkl->image[(i*pkl->width+j)*pkl->channel], pkl->channel);
			}else{
				for(k=0; k<pkl->channel; k++)
					wrk[(i*pkl->width+j)*pkl->channel+k] = PKL_COLOR_CLIP(pkl->image[(i*pkl->width+j)*pkl->channel+k]+weight);
			}
			err = pix>127 ? pix-255 : pix;

			for(s=0; s<col; s++){
				for(t=-xrange; t<=xrange; t++){
					tx = j+t;
					ty = i+s;
					if(tx<0 || tx>=pkl->width || ty>=pkl->height) continue;
					if (pt[s*row+t+xrange] < 0) continue;
					graypic[ty*pkl->width+tx] = PKL_COLOR_CLIP(graypic[ty*pkl->width+tx] + err * pt[s*row+t+xrange]);
				}
			}
		}
	}
	free(graypic);
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_contour
//=============================================================================
PKLExport int pkl_contour(PKLImage pkl)
{
	double dd=1.732;
	int i, j, k, s, ix[4], iy[4], dis[4], max;
	unsigned char *wrk, p, q;

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			ix[0]=j+1; iy[0]=i-1;	//右上
			ix[1]=j+1; iy[1]=i;		//右
			ix[2]=j+1; iy[2]=i+1;	//右下
			ix[3]=j;   iy[3]=i+1;	//下

			for(s=0; s<4; s++){
				dis[s]=0;
				if(ix[s]>=0 && ix[s]<pkl->width && iy[s]>=0 && iy[s]<pkl->height){
					for(k=0; k<pkl->channel; k++){
						p = pkl->image[(i*pkl->width+j)*pkl->channel+k];
						q = pkl->image[(iy[s]*pkl->width+ix[s])*pkl->channel+k];
						dis[s] += ((q-p) * (q-p));
					}
				}
			}
			max=0;
			for(s=0; s<4; s++)
				if(dis[s]>max) max=dis[s];
			
			for(k=0; k<pkl->channel; k++)
				wrk[(i*pkl->width+j)*pkl->channel+k] = PKL_COLOR_CLIP(255-sqrt((double)max)/dd);
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_focus
//=============================================================================
PKLExport int pkl_focus(PKLImage pkl, PKL_FOCUS type)
{
	unsigned char *wrk;
	int i, j, s, t, k, clipx, clipy, weight=1;
	int *matrix, color[PKL_CHANNEL];

	//select filter
	matrix=focus_filter[0].matrix;
	for(i=0; i<sizeof(focus_filter)/sizeof(struct PKL_FILTER); i++){
		if(type==focus_filter[i].type){
			matrix=focus_filter[i].matrix;
			break;
		}
	}

	for(i=0; i<9; i++)
		weight += matrix[i];
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			memset(color, 0, sizeof(color));
			for(s=0; s<3; s++){
				for(t=0; t<3; t++){
					clipx = (j+t)>=pkl->width  ? pkl->width-1  : j+t;
					clipy = (i+s)>=pkl->height ? pkl->height-1 : i+s;
					for(k=0; k<pkl->channel; k++)
						color[k] += pkl->image[(clipy*pkl->width+clipx) * pkl->channel + k] * matrix[t+s*3];
				}
			}
			
			for(k=0; k<pkl->channel; k++)
				wrk[(i*pkl->width+j)*pkl->channel+k] = PKL_COLOR_CLIP(color[k]/weight);
		}
	}

	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_emboss
//=============================================================================
PKLExport int pkl_emboss(PKLImage pkl, PKL_EMBOSS type)
{
	int i, *mask;

	//select filter
	mask=focus_filter[0].matrix;
	for(i=0; i<sizeof(emboss_filter)/sizeof(struct PKL_FILTER); i++){
		if(type==emboss_filter[i].type){
			mask = emboss_filter[i].matrix;
			break;
		}
	}
	return pkl_emboss2(pkl, mask, 3, 3, 1.0, 127);
}

//=============================================================================
// pkl_emboss2
//=============================================================================
PKLExport int pkl_emboss2(PKLImage pkl, int *mask, int row, int col, double factor, int offset)
{
	unsigned char *gray;
	int i, j, s, t, k, clipx, clipy, stock;

	gray = malloc(pkl->width*pkl->height);
	if(!gray) return(1);
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			stock=0;
			for(k=0; k<pkl->channel; k++)
				stock += pkl->image[(i*pkl->width+j)*pkl->channel+k];
			gray[i*pkl->width+j] = PKL_COLOR_CLIP(stock/pkl->channel);
		}
	}

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			stock=0;
			for(s=0; s<col; s++){
				for(t=0; t<row; t++){
					clipx = (j+t)>=pkl->width  ? pkl->width-1  : j+t;
					clipy = (i+s)>=pkl->height ? pkl->height-1 : i+s;
					stock += gray[clipy*pkl->width+clipx] * mask[t+s*row];
				}
			}
			memset(&pkl->image[(i*pkl->width+j)*pkl->channel], PKL_COLOR_CLIP(stock/factor + offset), pkl->channel);
		}
	}
	free(gray);
	return(0);
}

//=================================================================================
// pkl_swapcolor
//=================================================================================
static void swp_rbg(unsigned char *p){
	unsigned char r=p[0], g=p[1], b=p[2];
	p[0] = r;
	p[1] = b;
	p[2] = g;
}
static void swp_bgr(unsigned char *p){
	unsigned char r=p[0], g=p[1], b=p[2];
	p[0] = b;
	p[1] = g;
	p[2] = r;
}
static void swp_brg(unsigned char *p){
	unsigned char r=p[0], g=p[1], b=p[2];
	p[0] = g;
	p[1] = b;
	p[2] = r;
}
static void swp_grb(unsigned char *p){
	unsigned char r=p[0], g=p[1], b=p[2];
	p[0] = g;
	p[1] = r;
	p[2] = b;
}
static void swp_gbr(unsigned char *p){
	unsigned char r=p[0], g=p[1], b=p[2];
	p[0] = b;
	p[1] = r;
	p[2] = g;
}

PKLExport int pkl_swapcolor(PKLImage pkl, PKL_SWAPMODE mode)
{
	int i, j;
	void (*swaper[])(unsigned char *p) = {
		swp_rbg,
		swp_bgr,
		swp_brg,
		swp_grb,
		swp_gbr
	};

	if(pkl->color!=PKL_RGB) return(1);
	
	for(i=0; i<pkl->height; i++)
		for(j=0; j<pkl->width; j++)
			swaper[mode](&pkl->image[(i*pkl->width+j)*pkl->channel]);

	return(0);
}
