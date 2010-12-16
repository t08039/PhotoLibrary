#include "pikl_composite.h"

//=============================================================================
// pkl_composite
//=============================================================================
PKLExport int pkl_composite(PKLImage parent, PKLImage child, int xpos, int ypos, PKLColor transcolor)
{
	int tw, th, i, j, px, py, cx, cy;
	
	if(parent->channel != child->channel) return(1);
	if(child->width > parent->width || child->height > parent->height) return(1);
	if(xpos+child->width<0 || xpos>=parent->width ||
	   ypos+child->height<0 || ypos>=parent->height) return(0);
	
	tw = xpos<0 ? child->width+xpos : (xpos+child->width >= parent->width) ? parent->width-xpos : child->width;
	th = ypos<0 ? child->height+ypos : (ypos+child->height >= parent->height) ? parent->height-ypos : child->height;
	px = xpos<0 ? 0 : xpos;
	py = ypos<0 ? 0 : ypos;
	cx = xpos<0 ? -xpos : 0;
	cy = ypos<0 ? -ypos : 0;
	
	if(transcolor){
		//透明色が設定されている時
		int s, t;
		for(i=py,j=cy; i<th+py; i++,j++){
			for(s=px,t=cx; t<cx+tw; s++,t++){
				if( memcmp(&child->image[(j*child->width+t)*child->channel], transcolor->color, child->channel) ){
					memcpy(&parent->image[(i*parent->width+s)*parent->channel],
						   &child->image[(j*child->width+t)*child->channel], child->channel);
				}
			}
		}
	}else{
		//透明色が設定されていない時
		for(i=py,j=cy; i<th+py; i++,j++){
			memcpy(&parent->image[(i*parent->width+px)*parent->channel],
				   &child->image[(j*child->width+cx)*child->channel], tw*child->channel);
		}
	}
	return(0);
}

//=============================================================================
// pkl_alphablend
//=============================================================================
PKLExport int pkl_alphablend(PKLImage parent, PKLImage child, int xpos, int ypos, int alpha)
{
	int tw, th, i, j, k, pp, cp, px, py, cx, cy, s, t;
	
	if(parent->channel != child->channel) return(1);
	if(child->width > parent->width || child->height > parent->height) return(1);
	if(xpos+child->width<0 || xpos>=parent->width ||
	   ypos+child->height<0 || ypos>=parent->height) return(0);
	
	tw = xpos<0 ? child->width+xpos : (xpos+child->width >= parent->width) ? parent->width-xpos : child->width;
	th = ypos<0 ? child->height+ypos : (ypos+child->height >= parent->height) ? parent->height-ypos : child->height;
	px = xpos<0 ? 0 : xpos;
	py = ypos<0 ? 0 : ypos;
	cx = xpos<0 ? -xpos : 0;
	cy = ypos<0 ? -ypos : 0;
	
	if(alpha<0){
		//childの周囲のみにαを適用
		double alp;
		
		for(i=py,j=cy; i<th+py; i++,j++){
			for(s=px,t=cx; t<cx+tw; s++,t++){
				alp = sin(j*M_PI/child->height) * sin(t*M_PI/child->width);
				pp = (i*parent->width+s)*parent->channel;
				cp = (j*child->width+t)*child->channel;
				for(k=0; k<parent->channel; k++)
					parent->image[pp+k] =PKL_COLOR_CLIP(parent->image[pp+k]*(1.0-alp) + child->image[cp+k]*alp);
			}
		}
	}else{
		//child全体にαを適用
		int alp=PKL_COLOR_CLIP(alpha);
		const int limits=PKL_COLOR-1;
		
		for(i=py,j=cy; i<th+py; i++,j++){
			for(s=px,t=cx; t<cx+tw; s++,t++){
				pp = (i*parent->width+s)*parent->channel;
				cp = (j*child->width+t)*child->channel;
				for(k=0; k<parent->channel; k++)
					parent->image[pp+k] =PKL_COLOR_CLIP((parent->image[pp+k]*alp + child->image[cp+k]*(limits-alp)) / limits);
			}
		}
	}
	return(0);
}

//=============================================================================
// pkl_shadowframe
//=============================================================================
PKLExport int pkl_shadowframe(PKLImage pkl, int margin, PKLColor backcolor, PKLColor shadowcolor)
{
	unsigned char *img, back[PKL_CHANNEL], shadow[PKL_CHANNEL];
	int i, j, w, h;

	if(margin<=0) return(1);

	w = pkl->width;
	h = pkl->height;
	img = pkl->image;
	pkl->width = w + margin*3;
	pkl->height = h + margin*3;
	
	pkl->image = malloc(pkl->width*pkl->height*pkl->channel);
	if(!pkl->image) return(1);
	memset(pkl->image, 0, pkl->width*pkl->height*pkl->channel);

	//for(i=0; i<pkl->channel; i++){
	//	back[i] = (backcolor>>((pkl->channel-i)*8) & 0xFF);
	//	shadow[i] = (shadowcolor>>((pkl->channel-i)*8) & 0xFF);
	//}
	if(backcolor) memcpy(back, backcolor->color, pkl->channel);
	else          memset(back, 0xff, pkl->channel);

	if(shadowcolor) memcpy(shadow, shadowcolor->color, pkl->channel);
	else            memset(shadow, 0, pkl->channel);
	
	//背景色設定
	for(i=0; i<pkl->width*pkl->height*pkl->channel; i+=pkl->channel)
		memcpy(&pkl->image[i], back, pkl->channel);
	
	//フレーム設定
	for(i=margin*2; i<h+margin*2; i++)
		for(j=margin*2; j<w+margin*2; j++)
			memcpy(&pkl->image[(i*pkl->width+j)*pkl->channel], shadow, pkl->channel);
	
	//shadowframeを作る(単にブラー)
	pkl_gaussblur(pkl, 5.0);
	
	for(i=margin,j=0; i<h+margin; i++,j++)
		memcpy(&pkl->image[(i*pkl->width+margin)*pkl->channel], &img[(j*w)*pkl->channel], w*pkl->channel);
	
	free(img);
	return(0);
}


//=============================================================================
// pkl_randomshadow
//=============================================================================
PKLExport int pkl_randomshadow(PKLImage canvas, PKLImage pasteimage, int top, int left, int margin, PKLColor shadowcolor)
{
	PKLImage shadow;
	int i, j;
	unsigned char shcolor[PKL_CHANNEL];
	
	if(canvas->color != pasteimage->color) return(1);
	
	//シャドウフレームを作る
	shadow = pkl_canvas(pasteimage->width, pasteimage->height, pasteimage->color, NULL);
	if(!shadow) return(1);
	
	//for(i=0; i<shadow->channel; i++)
	//	shcolor[i] = (shadowcolor>>((shadow->channel-i)*8) & 0xFF);
	if(shadowcolor) memcpy(shcolor, shadowcolor->color, canvas->channel);
	else            memset(shcolor, 0, canvas->channel);
	
	for(i=0; i<pasteimage->height; i++)
		for(j=0; j<pasteimage->width; j++)
			memcpy(&shadow->image[(i*shadow->width+j)*shadow->channel], shcolor, shadow->channel);
	
	//出来上がったシャドウフレームをキャンバスに貼り付ける
	pkl_alphablend(canvas, shadow, top+margin, left+margin, -1);
	pkl_alphablend(canvas, shadow, top+margin, left+margin, -1);
	pkl_alphablend(canvas, shadow, top+margin, left+margin, -1);
	pkl_close(shadow);
	
	//オリジナルイメージを貼る
	pkl_composite(canvas, pasteimage, top, left, NULL);
	return(0);
}
