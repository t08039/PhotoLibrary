#include "pikl_divide.h"

//=============================================================================
// pkl_tileslit
//=============================================================================
PKLExport int pkl_tileslit(PKLImage pkl, int area, int shift)
{
	unsigned char *wrk;
	int i, j, k, tx, ty;
	int w, h, offsetx, offsety;

	if(shift==0) return(0);
	if(area<=1 || (area>pkl->width || area>pkl->height)) return(1);

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	
	//invert
	for(i=0; i<pkl->height*pkl->width*pkl->channel; i++)
		wrk[i] = (pkl->image[i] ^ 0xff);

	srand(1);
	for(i=0; i<pkl->height; i+=area){
		for(j=0; j<pkl->width; j+=area){
			//格子のずらし量
			offsetx = rand()%(shift*2+1)-shift;
			offsety = rand()%(shift*2+1)-shift;
			
			h = i+area>pkl->height ? pkl->height : i+area;
			for(k=i; k<h; k++){
				tx = j+offsetx<0 ? 0 : j+offsetx>=pkl->width ? pkl->width-1 : j+offsetx;
				ty = k+offsety<0 ? 0 : k+offsety>=pkl->height ? pkl->height-1 : k+offsety;
				w = tx+area > pkl->width ? pkl->width-tx : area;
				memcpy(&wrk[(ty*pkl->width+tx)*pkl->channel], &pkl->image[(k*pkl->width+j)*pkl->channel], w*pkl->channel);
			}
		}
	}

	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_splitframe
//=============================================================================
PKLExport int pkl_splitframe(PKLImage pkl, PKLColor backcolor, int wbs, int hbs, int margin, int frame)
{
	unsigned char *wrk;
	int i, j, k, s, t, u, tx, ty;
	int offsetx, offsety, width, height, fw, fh;
	int warea, harea;
	int shift=5;

	if(wbs<=0 || hbs<=0) return(1);
	if(margin<0) return(1);
	if(frame<1) return(1);

	//1つの分割エリアのイメージサイズ
	warea = pkl->width / wbs;
	harea = pkl->height / hbs;
	//1つのエリアのサイズ(フレームサイズを含む)
	fw = warea + frame*2;
	fh = harea + frame*2;
	//最終的なキャンバスの大きさ
	width = fw * wbs + margin*2;
	height = fh * hbs + margin*2;

	wrk = malloc(width*height*pkl->channel);
	if(!wrk) return(1);
	
	//背景色の設定
	if(backcolor){
		for(i=0; i<width*height; i++)
			memcpy(&wrk[i*pkl->channel], backcolor->color, pkl->channel);
	}else{
		memset(wrk, 0xff, width*height*pkl->channel);
	}

	srand(1);
	for(i=margin,s=0; i<height-margin; i+=fh,s+=harea){
		for(j=margin,t=0; j<width-margin; j+=fw,t+=warea){
			offsetx = rand()%(shift*2+1)-shift;
			offsety = rand()%(shift*2+1)-shift;
			
			//フォトフレーム
			k = i+offsety<0 ? 0 : i+offsety+fh>height ? height-fh : i+offsety;
			for(ty=k; ty<k+fh; ty++){
				tx = j+offsetx<0 ? 0 : j+offsetx+fw>width ? width-fw : j+offsetx;
				//背景色(白)
				memset(&wrk[(ty*width+tx)*pkl->channel], 0xFF, fw*pkl->channel);
				//天地のフレーム
				if(ty==k || ty==k+fh-1)
					memset(&wrk[(ty*width+tx)*pkl->channel], 0x00, fw*pkl->channel);
				//左右のフレーム
				memset(&wrk[(ty*width+tx)*pkl->channel], 0x00, pkl->channel);
				memset(&wrk[(ty*width+tx+fw-1)*pkl->channel], 0x00, pkl->channel);
			}
			//イメージ
			k = i+offsety<0 ? frame : i+offsety+fh>height ? height-fh+frame : i+offsety+frame;
			for(ty=k,u=s; ty<k+harea; ty++,u++){
				tx = j+offsetx<0 ? frame : j+offsetx+fw>width ? width-fw+frame : j+offsetx+frame;
				memcpy(&wrk[(ty*width+tx)*pkl->channel], &pkl->image[(u*pkl->width+t)*pkl->channel], warea*pkl->channel);
			}
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	pkl->width = width;
	pkl->height = height;
	return(0);
}
