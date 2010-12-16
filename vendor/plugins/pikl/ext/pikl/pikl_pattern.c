#include "pikl_pattern.h"

static int tile_rectangle(PKLImage pkl, int msx, int msy, unsigned char level);
static int tile_pattern(PKLImage pkl, PKL_TILE_TYPE type, int pw, int ph, unsigned char level);
static int tile_pattern_draw(PKLImage pkl, double *wd, int pw, int ph, unsigned char level);
static int tile_brick(PKLImage pkl, int msx, int msy, unsigned char level);

static void pattern_hexagon(int ph, double *wd, double ww, double hh);
static void pattern_diamond(int ph, double *wd, double ww, double hh);
static void pattern_circle(int ph, double *wd, double ww, double hh);
static void pattern_brick(int ph, double *wd, double ww, double hh);

static int pkl_pattern_draw_line(PKLImage pkl, double *wd, int pw, int ph);
static int pkl_pattern_draw_average(PKLImage pkl, double *wd, int pw, int ph);

//=============================================================================
// pkl_pattern
//=============================================================================
PKLExport int pkl_pattern(PKLImage pkl, int pw, int ph, PKL_PATTERN_TYPE mode, PKL_PAINT_TYPE type)
{
	double ww, hh, *wd;
	int result;
	
	if(pw<1 || ph<1) return(1);
	
	wd = malloc(sizeof(double)*ph);
	if(!wd) return(1);
	
	ww = (double)pw;
	hh = (double)ph;
	
	/* パターン座標の構築 */
	switch(mode){
		case PKL_PATTERN_DIAMOND:
			pattern_diamond(ph, wd, ww, hh);
			break;
		case PKL_PATTERN_CIRCLE:
			pattern_circle(ph, wd, ww, hh);
			break;
		case PKL_PATTERN_BRICK:
			pattern_brick(ph, wd, ww, hh);
			break;
		case PKL_PATTERN_HEXAGON:
		default:
			pattern_hexagon(ph, wd, ww, hh);
	}
	
	/* 塗りタイプ別の処理 */
	switch(type){
		case PKL_PAINT_AVE:
			result = pkl_pattern_draw_average(pkl, wd, pw, ph);
			break;
		case PKL_PAINT_LINE:
		default:
			result = pkl_pattern_draw_line(pkl, wd, pw, ph);
	}

	free(wd);
	return(result);
}

//=============================================================================
// pattern_hexagon
//=============================================================================
void pattern_hexagon(int ph, double *wd, double ww, double hh)
{
	int i;
	for(i=0; i<ph; i++){
		if((double)i < hh/6.0){
			wd[i] = ww * (double)i*6.0/hh;
		}else
		if((double)i < hh/2.0){
			wd[i] = ww;
		}else
		if((double)i < hh*2.0/3.0){
			wd[i] = ww*4.0 - ww*(double)i*6.0/hh;
		}else{
			wd[i] = 0.0;
		}
	}
}

//=============================================================================
// pattern_diamond
//=============================================================================
void pattern_diamond(int ph, double *wd, double ww, double hh)
{
	int i;
	for(i=0; i<ph; i++){
		if((double)i < hh/2.0){
			wd[i] = ww*(double)i*2.0/hh;
		}else{
			wd[i] = ww*2.0 - ww*(double)i*2.0/hh;
		}
	}
}

//=============================================================================
// pattern_circle
//=============================================================================
void pattern_circle(int ph, double *wd, double ww, double hh)
{
	int i;
	for(i=0; i<ph; i++){
		if((double)i < hh/2.0){
			wd[i] = ww*(double)i*2.0/hh - (ww/8.0)*sin((double)i*4.0/hh*M_PI);
		}else{
			wd[i] = ww*2.0 - ww*(double)i*2.0/hh + (ww/8.0)*sin((double)(i-hh/2.0)*4.0/hh*M_PI);
		}
	}
}

//=============================================================================
// pattern_brick
//=============================================================================
void pattern_brick(int ph, double *wd, double ww, double hh)
{
	int i;
	for(i=0; i<ph; i++){
		if((double)i < hh/2.0){
			wd[i] = ww;
		}else{
			wd[i] = 0.0;
		}
	}
}

//=============================================================================
// pkl_pattern_draw_line
//=============================================================================
int pkl_pattern_draw_line(PKLImage pkl, double *wd, int pw, int ph)
{
	unsigned char clipcolor[PKL_CHANNEL], *p;
	int i, j, s, idx, k, color[PKL_CHANNEL];
	double ww, hh, rm;
	int count, ptn=0;

	if(pw<0 || pw>pkl->width || ph<0 || ph>pkl->height) return(1);
	
	ww = (double)pw;
	hh = (double)ph;

	//x方向に母点分ずつずらしていく
	for(j=0; j<pkl->width+pw; j+=pw){
		//パターンの切り替え
		ptn = !ptn;

		for(i=0; i<pkl->height; i++){
			memset(color, 0, sizeof(color));
			count=0;
			
			//パターン配列のインデックスを算出
			//(余剰を算出すれば、パターン配列を順番に処理できるから)
			idx = i % ph;
			
			//パターンを幅=pwの範囲で縦方向に見るので
			//パターン距離(wd)をpwから引くことで横方向の距離を算出できる
			//パターンは繰り返しなので、pw++ごとに切り替えればよい
			rm = ptn ? wd[idx] : ww-wd[idx];
			
			//jをパターンの中心として、両側へ広がりのある領域のピクセル平均を取る
			for(s=j-rm; s<=j+rm; s++){
				if(s<0 || s>=pkl->width) continue;
				p = &pkl->image[(i*pkl->width+s)*pkl->channel];
				for(k=0; k<pkl->channel; k++)
					color[k] += *p++;
				count++;
			}
			if(count==0) continue;
			for(k=0; k<pkl->channel; k++)
				clipcolor[k] = PKL_COLOR_CLIP(color[k]/count);

			//作った色を横方向に適用する
			for(s=j-rm; s<=j+rm; s++){
				if(s<0 || s>=pkl->width) continue;
				memcpy(&pkl->image[(i*pkl->width+s)*pkl->channel], clipcolor, pkl->channel);
			}
		}
	}
	return(0);
}

//=============================================================================
// pkl_pattern_draw_average
//=============================================================================
int pkl_pattern_draw_average(PKLImage pkl, double *wd, int pw, int ph)
{
	unsigned char clipcolor[PKL_CHANNEL], *p;
	int i, j, s, t, k, color[PKL_CHANNEL];
	int count, ys, idx, ptn=0;
	double ww, hh, rm;
	
	if(pw<0 || pw>pkl->width || ph<0 || ph>pkl->height) return(1);
	
	ww = (double)pw;
	hh = (double)ph;

	for(j=0; j<pkl->width+pw; j+=pw){
		ptn = !ptn;
		ys = (ptn) ? 0 : (-ph/2);

		for(i=ys; i<pkl->height; i+=ph){
			memset(color, 0, sizeof(color));
			count=0;
			rm=wd[0];
			
			for(t=i; t<i+ph; t++){
				if(t<0 || t>=pkl->height) continue;
				
				idx = t % ph;
				rm = ptn ? wd[idx] : ww-wd[idx];
				for(s=j-rm; s<=j+rm; s++){
					if(s<0 || s>=pkl->width) continue;
					p = &pkl->image[(t*pkl->width+s)*pkl->channel];
					for(k=0; k<pkl->channel; k++)
						color[k] += *p++;
					count++;
				}
			}
			
			/* パターン領域内の平均算出 */
			if(count==0) continue;
			for(k=0; k<pkl->channel; k++)
				clipcolor[k] = PKL_COLOR_CLIP(color[k]/count);
			
			/* パターン領域を同一色で塗りつぶす */
			for(t=i; t<i+ph; t++){
				if(t<0 || t>=pkl->height) continue;
				idx = t % ph;
				rm = ptn ? wd[idx] : ww-wd[idx];
				
				for(s=j-rm; s<=j+rm; s++){
					if(s<0 || s>=pkl->width) continue;
					memcpy(&pkl->image[(t*pkl->width+s)*pkl->channel], clipcolor, pkl->channel);
				}
				
				//境界線
				//if(j-rm-1>=0)
				//	memset(&pkl->image[(t*pkl->width+(int)(j-rm-1))*pkl->channel], 0, pkl->channel);
				//if(j+rm+1<pkl->width)
				//	memset(&pkl->image[(t*pkl->width+(int)(j+rm+1))*pkl->channel], 0, pkl->channel);
				//if(t-1>=0)
				//	memset(&pkl->image[((t-1)*pkl->width+s)*pkl->channel], 0, pkl->channel);
				//if(t+1<pkl->height)
				//	memset(&pkl->image[((t+1)*pkl->width+s)*pkl->channel], 0, pkl->channel);
			}
		}
	}
	return(0);
}

//=============================================================================
// pkl_tile
//=============================================================================
PKLExport int pkl_tile(PKLImage pkl, PKL_TILE_TYPE type, int msx, int msy, unsigned char level)
{
	if(msx<=0 || msx>pkl->width || msy<=0 || msy>pkl->height)
		return(1);
	
	switch(type){
		case PKL_TILE_RECT:
			return tile_rectangle(pkl, msx, msy, level);
		case PKL_TILE_HEXAGON:
		case PKL_TILE_DIAMOND:
		case PKL_TILE_CIRCLE:
			return tile_pattern(pkl, type, msx, msy, level);
		case PKL_TILE_BRICK:
			return tile_brick(pkl, msx, msy, level);
	}
	return(1);
}

//=============================================================================
// tile_brick
//=============================================================================
static int tile_brick(PKLImage pkl, int msx, int msy, unsigned char level)
{
	unsigned char *wrk, *p;
	int mpx, mpy;	//モザイクの大きさ
	int mlx, mly;	//画像の分割数
	int i, j, k, sx, sy, ex, ey, px, py, count, offset;
	long color[PKL_CHANNEL];

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);

	mpx = (msx<1) ? 1 : (msx>=pkl->width) ? pkl->width-1 : msx;
	mpy = (msy<1) ? 1 : (msy>=pkl->height) ? pkl->height-1 : msy;
	mlx = (pkl->width+mpx-1)/mpx;
	mly = (pkl->height+mpy-1)/mpy;
	
	for(i=0; i<=mly; i++){
		offset = (i%2) ? 0 : mpx/2;
		for(j=0; j<=mlx; j++){
			sx = j*mpx - offset;
			sx = sx<0 ? 0 : sx;
			sy = i*mpy;
			ex = sx+mpx>pkl->width ? pkl->width : sx+mpx;
			ey = sy+mpy>pkl->height ? pkl->height : sy+mpy;
			
			//1セルの総和算出
			count=0;
			memset(color, 0, sizeof(color));
			for(py=sy; py<ey; py++){
				for(px=sx; px<ex; px++){
					count++;
					for(k=0; k<pkl->channel; k++)
						color[k] += pkl->image[(py*pkl->width+px)*pkl->channel+k];
				}
			}
			if(!count) continue;
			
			//セルの平均色を算出して、セル内のピクセルに適用
			for(k=0; k<pkl->channel; k++)
				color[k] = PKL_COLOR_CLIP(color[k]/count);
			for(py=sy; py<ey; py++){
				for(px=sx; px<ex; px++){
					for(k=0; k<pkl->channel; k++)
						wrk[(py*pkl->width+px)*pkl->channel+k] = color[k];
				}
			}
			
			//平均色で塗り終わったら枠線を描く
			//横方向
			for(px=sx; px<ex; px++){
				p = &wrk[(sy*pkl->width+px)*pkl->channel];
				for(k=0; k<pkl->channel; k++,p++)
					*p = PKL_MIN(0xff, *p+level);
				
				p = &wrk[((ey-1)*pkl->width+px)*pkl->channel];
				for(k=0; k<pkl->channel; k++,p++)
					*p = PKL_MAX(0, *p-level);
			}
			//縦方向
			for(py=sy; py<ey; py++){
				p = &wrk[(py*pkl->width+sx)*pkl->channel];
				for(k=0; k<pkl->channel; k++,p++)
					*p = PKL_MIN(0xff, *p+level);
				
				p = &wrk[(py*pkl->width+ex-1)*pkl->channel];
				for(k=0; k<pkl->channel; k++,p++)
					*p = PKL_MAX(0, *p-level);
			}
			
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// tile_rectangle
//=============================================================================
static int tile_rectangle(PKLImage pkl, int msx, int msy, unsigned char level)
{
	unsigned char *p;
	int i, j, k;

	//モザイク化
	if(pkl_mosaic(pkl, msx, msy)) return(1);

	//横方向(light)
	for(i=0; i<pkl->height; i+=msy){
		for(j=0; j<pkl->width; j++){
			p = &pkl->image[(i*pkl->width+j)*pkl->channel];
			for(k=0; k<pkl->channel; k++,p++)
				*p = PKL_MIN(0xff, *p+level);
		}
	}

	//横方向(dark)
	for(i=msy-1; i<pkl->height; i+=msy){
		for(j=0; j<pkl->width; j++){
			p = &pkl->image[(i*pkl->width+j)*pkl->channel];
			for(k=0; k<pkl->channel; k++,p++)
				*p = PKL_MAX(0, *p-level);
		}
	}

	//縦方向(light)
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j+=msx){
			p = &pkl->image[(i*pkl->width+j)*pkl->channel];
			for(k=0; k<pkl->channel; k++,p++)
				*p = PKL_MIN(0xff, *p+level);
		}
	}

	//縦方向(dark)
	for(i=0; i<pkl->height; i++){
		for(j=msx-1; j<pkl->width; j+=msx){
			p = &pkl->image[(i*pkl->width+j)*pkl->channel];
			for(k=0; k<pkl->channel; k++,p++)
				*p = PKL_MAX(0, *p-level);
		}
	}

	return(0);
}

//=============================================================================
// tile_pattern
//=============================================================================
static int tile_pattern(PKLImage pkl, PKL_TILE_TYPE type, int pw, int ph, unsigned char level)
{
	double ww, hh, *wd;
	int result=1;
	
	wd = malloc(sizeof(double)*ph);
	if(!wd) return(1);
	
	ww = (double)pw;
	hh = (double)ph;
	
	/* パターン座標の構築 */
	switch(type){
		case PKL_TILE_HEXAGON:
			pattern_hexagon(ph, wd, ww, hh); break;
		case PKL_TILE_DIAMOND:
			pattern_diamond(ph, wd, ww, hh); break;
		case PKL_TILE_CIRCLE:
			pattern_circle(ph, wd, ww, hh); break;
		default:
			;;
	}
	
	//パターンで塗りつぶしてから枠を描く
	if(!pkl_pattern_draw_average(pkl, wd, pw, ph))
		result = tile_pattern_draw(pkl, wd, pw, ph, level);
	free(wd);
	return(result);
}

//=============================================================================
// tile_pattern_draw
//=============================================================================
static int tile_pattern_draw(PKLImage pkl, double *wd, int pw, int ph, unsigned char level)
{
	unsigned char *p;
	int i, j, t, k, pos;
	int ys, idx, ptn=0;
	double ww, hh, rm;
	
	ww = (double)pw;
	hh = (double)ph;

	for(j=0; j<pkl->width+pw; j+=pw){
		ptn = !ptn;
		ys = (ptn) ? 0 : (-ph/2);
		for(i=ys; i<pkl->height; i+=ph){
			rm=wd[0];
			
			for(t=i; t<i+ph; t++){
				if(t<0 || t>=pkl->height) continue;
				idx = t % ph;
				rm = ptn ? wd[idx] : ww-wd[idx];
				
				//first=light
				pos = j - rm;
				if(pos>=0 && pos<pkl->width){
					p = &pkl->image[(t*pkl->width+pos)*pkl->channel];
					for(k=0; k<pkl->channel; k++,p++)
						*p = PKL_MIN(0xff, *p+level);
				}
				
				//last=dark
				pos = j + rm - 1;
				if(pos>=0 && pos<pkl->width){
					p = &pkl->image[(t*pkl->width+pos)*pkl->channel];
					for(k=0; k<pkl->channel; k++,p++)
						*p = PKL_MAX(0, *p-level);
				}
			}
		}
	}
	return(0);
}

//=============================================================================
// pkl_mosaic
//=============================================================================
PKLExport int pkl_mosaic(PKLImage pkl, int msx, int msy)
{
	unsigned char *wrk;
	int mpx, mpy;	//モザイクの大きさ
	int mlx, mly;	//画像の分割数
	int i, j, k, sx, sy, ex, ey, px, py, mm;
	long color[PKL_CHANNEL];

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);

	mpx = (msx<1) ? 1 : (msx>=pkl->width) ? pkl->width-1 : msx;
	mpy = (msy<1) ? 1 : (msy>=pkl->height) ? pkl->height-1 : msy;
	mlx = (pkl->width+mpx-1)/mpx;
	mly = (pkl->height+mpy-1)/mpy;
	
	for(i=0; i<=mly; i++){
		for(j=0; j<=mlx; j++){
			sx = j*mpx;
			sy = i*mpy;
			ex = sx+mpx>pkl->width ? pkl->width : sx+mpx;
			ey = sy+mpy>pkl->height ? pkl->height : sy+mpy;
			
			//1セルの総和算出
			mm=0;
			memset(color, 0, sizeof(color));
			for(py=sy; py<ey; py++){
				for(px=sx; px<ex; px++){
					mm++;
					for(k=0; k<pkl->channel; k++)
						color[k] += pkl->image[(py*pkl->width+px)*pkl->channel+k];
				}
			}
			if(!mm) continue;
			
			//セルの平均色を算出して、セル内のピクセルに適用
			for(k=0; k<pkl->channel; k++)
				color[k] = PKL_COLOR_CLIP(color[k]/mm);
			for(py=sy; py<ey; py++){
				for(px=sx; px<ex; px++){
					for(k=0; k<pkl->channel; k++)
						wrk[(py*pkl->width+px)*pkl->channel+k] = color[k];
				}
			}
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_circle
//=============================================================================
PKLExport int pkl_circle(PKLImage pkl, int radius)
{
	unsigned char *wrk, *image;
	int mp;	//モザイクの大きさ
	int mlx, mly;	//画像の分割数
	int i, j, k, sx, sy, ex, ey, px, py, mm;
	long stock[PKL_CHANNEL];
	struct _PKLColor color;
	
	mp = radius*2;	//直径
	if(mp<3 || mp>=pkl->width || mp>=pkl->height)
		return(1);

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0xff, pkl->width*pkl->height*pkl->channel);

	image = pkl->image;
	pkl->image=wrk;

	mlx = (pkl->width+mp-1)/mp;
	mly = (pkl->height+mp-1)/mp;
	
	for(i=0; i<=mly; i++){
		for(j=0; j<=mlx; j++){
			sx = j*mp;
			sy = i*mp;
			ex = sx+mp>pkl->width ? pkl->width : sx+mp;
			ey = sy+mp>pkl->height ? pkl->height : sy+mp;
			
			//1セルの総和算出
			mm=0;
			memset(stock, 0, sizeof(stock));
			for(py=sy; py<ey; py++){
				for(px=sx; px<ex; px++){
					mm++;
					for(k=0; k<pkl->channel; k++)
						stock[k] += image[(py*pkl->width+px)*pkl->channel+k];
				}
			}
			
			if(!mm) continue;
			for(k=0; k<pkl->channel; k++)
				color.color[k] = PKL_COLOR_CLIP(stock[k]/mm);
			
			draw_circle(pkl, sx+radius, sy+radius, radius, &color);
		}
	}
	
	free(image);
	return(0);
}

//**あまり意味ないので削除
////=============================================================================
//// pkl_pixelate
////=============================================================================
//PKLExport int pkl_pixelate(PKLImage pkl, int ms)
//{
//	unsigned char *wrk;
//	int mpx, mpy;	//モザイクの大きさ
//	int mlx, mly;	//画像の分割数
//	int i, j, k, sx, sy, ex, ey, px, py, mm;
//	long color[PKL_CHANNEL];
//	int total;
//	unsigned char p;
//
//	wrk = malloc(pkl->width*pkl->height*pkl->channel);
//	if(!wrk) return(1);
//
//	mpx = (ms<1) ? 1 : (ms>=pkl->width) ? pkl->width-1 : ms;
//	mpy = (ms<1) ? 1 : (ms>=pkl->height) ? pkl->height-1 : ms;
//	mlx = (pkl->width+mpx-1)/mpx;
//	mly = (pkl->height+mpy-1)/mpy;
//	
//	for(i=0; i<=mly; i++){
//		for(j=0; j<=mlx; j++){
//			sx = j*mpx;
//			sy = i*mpy;
//			ex = sx+mpx>pkl->width ? pkl->width : sx+mpx;
//			ey = sy+mpy>pkl->height ? pkl->height : sy+mpy;
//			
//			//1セルの総和算出
//			mm=0;
//			memset(color, 0, sizeof(color));
//			for(py=sy; py<ey; py++){
//				for(px=sx; px<ex; px++){
//					mm++;
//					for(k=0; k<pkl->channel; k++)
//						color[k] += pkl->image[(py*pkl->width+px)*pkl->channel+k];
//				}
//			}
//			if(!mm) continue;
//			
//			//セルの平均色を算出
//			for(k=0; k<pkl->channel; k++)
//				color[k] = PKL_COLOR_CLIP(color[k]/mm);
//			
//			total=0;
//			for(k=0; k<pkl->channel; k++)
//				total += color[k];
//			total /=pkl->channel;
//			p = total<128 ? 0 : 255;
//			
//			for(py=sy; py<ey; py++){
//				for(px=sx; px<ex; px++){
//					for(k=0; k<pkl->channel; k++)
//						wrk[(py*pkl->width+px)*pkl->channel+k] = p;
//				}
//			}
//		}
//	}
//	
//	free(pkl->image);
//	pkl->image = wrk;
//	return(0);
//}
//
//=============================================================================
// pkl_grid
//=============================================================================
PKLExport int pkl_grid(PKLImage pkl, int msx, int msy, int color)
{
	unsigned char *p;
	int i, j, k, mpx, mpy;
	
	//一旦モザイク化
	if(pkl_mosaic(pkl, msx, msy)) return(1);
	
	mpx = (msx<1) ? 1 : (msx>=pkl->width) ? pkl->width-1 : msx;
	mpy = (msy<1) ? 1 : (msy>=pkl->height) ? pkl->height-1 : msy;
	
	for(i=0; i<pkl->height-1; i++){
		if(i%mpy==0){
			for(j=0; j<pkl->width; j++){
				p = &pkl->image[(i*pkl->width+j)*pkl->channel];
				for(k=0; k<pkl->channel; k++){
					*p = PKL_COLOR_CLIP(*p+color);
					p++;
				}
			}
		}
		for(j=mpx; j<pkl->width; j+=mpx){
			p = &pkl->image[(i*pkl->width+j)*pkl->channel];
			for(k=0; k<pkl->channel; k++){
				*p = PKL_COLOR_CLIP(*p+color);
				p++;
			}
		}
	}
	
	return(0);
}
