#include "pikl_draw.h"

//=============================================================================
//	円を描く
//	x,y: 円の中心座標
//	radius: 半径
//	color: 塗り色(unsigned char x[pkl->channel]の領域を持つポインタ)
//=============================================================================
PKLExport int draw_circle(PKLImage pkl, int x, int y, int radius, PKLColor color)
{
	int sx, sy, ex, ey, i, j, k, t, s;
	int is, os, dimm;
	const int DIV = 3;
	
	if(radius <= 0) return(1);
	if(x+radius<0 || x-radius>=pkl->width || y+radius<0 || y-radius>=pkl->height) return(1);

	dimm = DIV*DIV;
	
	//矩形座標を算出
	sx = x-radius;
	sy = y-radius;
	ex = x+radius;
	ey = y+radius;
	
	for(i=sy; i<ey; i++){
		if(i<0 || i>=pkl->height) continue;
		
		for(j=sx; j<ex; j++){
			if(j<0 || j>=pkl->width) continue;
			
			os=0;
			is=0;
			
			//こっちはいい感じになるけど、1pix少ない円になる
			//for(t=-area; t<=area; t++){)
			//	for(s=-area; s<=area; s++){
			
			//こっちは正確なサイズにできる
			for(t=0; t<DIV; t++){
				for(s=0; s<DIV; s++){
					double dx = (double)s/(double)DIV;
					double dy = (double)t/(double)DIV;
					
					//if((j+dx-x)*(j+dx-x)+(i+dy-y)*(i+dy-y) > (double)(radius*radius) +0.5)
					//if((j+dx-x)*(j+dx-x)+(i+dy-y)*(i+dy-y) >= (double)((radius+0.5)*radius))
					//if((j+dx-x)*(j+dx-x)+(i+dy-y)*(i+dy-y) > (double)((radius+0.5)*(radius+0.5)))
					if((j+dx-x)*(j+dx-x)+(i+dy-y)*(i+dy-y) > (double)((radius+0.5)*(radius-0.5)))
						os++;
					else
						is++;
				}
			}
			
			if(os==dimm)
				continue;
			if(is==dimm){
				memcpy(&pkl->image[(i*pkl->width+j)*pkl->channel], color->color, pkl->channel);
				continue;
			}
			
			for(k=0; k<pkl->channel; k++){
				pkl->image[(i*pkl->width+j)*pkl->channel+k] =
					PKL_COLOR_CLIP(pkl->image[(i*pkl->width+j)*pkl->channel+k] * os/dimm + color->color[k] * is/dimm);
			}
		}
	}
	return(0);
}


//=============================================================================
//	円を描く(AA)
//=============================================================================
PKLExport int draw_circle_aa(PKLImage pkl, int x, int y, int radius, PKLColor color, int aa)
{
	int sx, sy, ex, ey, i, j, k, t, s;
	int is, os, dimm, DIV;
	
	if(radius <= 0) return(1);
	if(x+radius<0 || x-radius>=pkl->width || y+radius<0 || y-radius>=pkl->height) return(1);
	if(aa < 3) return(1);

	DIV=aa;
	dimm = DIV*DIV;
	
	//矩形座標を算出
	sx = x-radius - DIV;
	sy = y-radius - DIV;
	ex = x+radius + DIV;
	ey = y+radius + DIV;
	
	for(i=sy; i<ey; i++){
		if(i<0 || i>=pkl->height) continue;
		
		for(j=sx; j<ex; j++){
			if(j<0 || j>=pkl->width) continue;
			
			os=0;
			is=0;
			for(t=0; t<DIV; t++){
				for(s=0; s<DIV; s++){
					double dx = (double)s/(double)DIV;
					double dy = (double)t/(double)DIV;
					
					//if((j-x+s)*(j-x+s)+(i-y+t)*(i-y+t) > (double)((radius+0.5)*(radius-0.5)))
					if((j-x+s+dx)*(j-x+s+dx)+(i-y+t+dy)*(i-y+t+dy) > (double)((radius+0.5)*(radius-0.5)))
						os++;
					else
						is++;
				}
			}
			
			if(os==dimm)
				continue;
			if(is==dimm){
				memcpy(&pkl->image[(i*pkl->width+j)*pkl->channel], color->color, pkl->channel);
				continue;
			}
			
			for(k=0; k<pkl->channel; k++){
				pkl->image[(i*pkl->width+j)*pkl->channel+k] =
					PKL_COLOR_CLIP(pkl->image[(i*pkl->width+j)*pkl->channel+k] * os/dimm + color->color[k] * is/dimm);
			}
		}
	}
	return(0);
}

