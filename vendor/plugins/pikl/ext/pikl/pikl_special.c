#include "pikl_special.h"

//=============================================================================
// pkl_edgeposter
//=============================================================================
PKLExport int pkl_edgeposter(PKLImage pkl, int level, double low, double high, double coeff, int edge)
{
	if(pkl_posterize(pkl, level)) return(1);
	if(pkl_noisecut(pkl)) return(1);
	//if(pkl_brightness(pkl, up)) return(1);
	if(pkl_level(pkl, low, high, coeff)) return(1);
	if(pkl_edgepaint(pkl, edge)) return(1);
	return(0);
}

//=============================================================================
// pkl_crayon
//=============================================================================
PKLExport int pkl_crayon(PKLImage pkl, double factor, int bv1, int bv2, int alpha)
{
	PKLImage img;
	
	//オリジナルのコピーを取っておく
	img = pkl_dup(pkl);
	
	//オリジナルから輪郭抽出
	pkl_rinkaku(pkl, factor);
	
	//レベル補正で鮮やかさを増す(本当は彩度をいじる？)
	pkl_level(img, 1.5, 1.5, 1.0);
	
	//拡散処理
	if(bv1>1)
		pkl_noise(img, bv1);
	if(bv2>1)
		pkl_noise(img, bv2);
	
	//合成
	pkl_alphablend(pkl, img, 0, 0, alpha);
	
	pkl_close(img);
	return(0);
}

//=============================================================================
// rinkaku
//=============================================================================
PKLExport int pkl_rinkaku(PKLImage pkl, double factor)
{
	PKLImage g1, g2;
	unsigned char *gray;
    double v, ff[PKL_COLOR];
	int edg, sbl, i, j, k, store;
	int mask25[5*5] =
			{ -1, -2, -3, -2, -1,
			  -2, -3, -4, -3, -2,
			  -3, -4, 60, -4, -3,
			  -2, -3, -4, -3, -2,
			  -1, -2, -3, -2, -1 };

	if(factor<5.0 || factor>80.0) return(1);

	for(i=0; i<PKL_COLOR; i++)
		ff[i] = factor * sin((double)i * M_PI/160);
	
	//グレーイメージを作る
	gray = malloc(pkl->width*pkl->height);
	if(!gray) return(1);
	
	/* convert grayscale */
	for(i=0; i<pkl->width*pkl->height; i++){
		store=0;
		for(k=0; k<pkl->channel; k++)
			store += pkl->image[i*pkl->channel+k];
		gray[i] = PKL_COLOR_CLIP(store/pkl->channel);
	}
	
	//オリジナルのコピー作成
	g1 = pkl_dup(pkl);
	g2 = pkl_dup(pkl);
	
	//コピー1に対して
	//	グレースケール化→輪郭抽出→レベル補正
	//pkl_gray(g1, PKL_GRAY_AVE);
		free(g1->image);
		g1->image = malloc(pkl->width*pkl->height);
		memcpy(g1->image, gray, pkl->width*pkl->height);
		g1->channel=1;
		g1->color=PKL_GRAYSCALE;
	//pkl_edge(g1, 10);
	pkl_emboss2(g1, mask25, 5, 5, 1.0, 255);
	pkl_level(g1, 0.015, 0.015, 1.0);

	//コピー2対して
	//	輪郭抽出→グレースケール化→レベル補正
	pkl_contour(g2);
		//pkl_gray(g2, PKL_GRAY_AVE);
		free(g2->image);
		g2->image = malloc(pkl->width*pkl->height);
		memcpy(g2->image, gray, pkl->width*pkl->height);
		g2->channel=1;
		g2->color=PKL_GRAYSCALE;
	pkl_level(g2, 1.5, 1.5, 1.0);
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			edg = 255 - g1->image[i*pkl->width+j];
			sbl = 255 - g2->image[i*pkl->width+j];
			v = sqrt(edg * sbl);
			v = v - ff[(int)v];
			for(k=0; k<pkl->channel; k++)
				pkl->image[(i*pkl->width+j)*pkl->channel+k] = PKL_COLOR_CLIP(255-v);
		}
	}

	free(gray);
	pkl_close(g1);
	pkl_close(g2);
	return(0);
}

//=============================================================================
// pkl_hydrangea
//=============================================================================
PKLExport int pkl_hydrangea(PKLImage pkl, int zone, double low, double high, double coeff, double mil, double env)
{
	//ボロノイ分割
	if(pkl_voronoi_zone(pkl, zone, NULL, 0)) return(1);
	
	//レベル補正
	if(pkl_level(pkl, low, high, coeff)) return(1);
	
	//カラーエンボス
	if(pkl_color_emboss(pkl, mil, env)) return (1);
	
	return(0);
}

//=============================================================================
// pkl_alphaedge
//=============================================================================
PKLExport int pkl_alphaedge(PKLImage pkl, int level, double low, double high, double coeff, int threshold, int alpha)
{
	PKLImage img;
	
	//ポスタライズ＆ノイズ除去
	if(pkl_posterize(pkl, level)) return(1);
	if(pkl_noisecut(pkl)) return(1);
	
	//コピー
	img = pkl_dup(pkl);
	//コピーをレベル補正
	if(pkl_level(img, low, high, coeff)) return(1);
	
	//オリジナルを単純カラーアップ
	if(pkl_brightness(pkl, 50)) return(1);
	//輪郭抽出
	if(pkl_edge(pkl, threshold)){
		pkl_close(img);
		return(1);
	}

	//合成
	pkl_alphablend(pkl, img, 0, 0, alpha);
	
	pkl_close(img);
	return(0);
}

//=============================================================================
// pkl_toy
//=============================================================================
PKLExport int pkl_toy(PKLImage pkl, PKLColor color, int trans, int radius, int x, int y, int rate, int weight)
{
	if(pkl_vv(pkl, color, trans, radius, x, y))
		return(1);
		
	if(pkl_contrast(pkl, rate))
		return(1);
		
	if(pkl_blur(pkl, weight))
		return(1);
	
	return(0);
}
