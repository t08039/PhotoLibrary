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
	
	//�I���W�i���̃R�s�[������Ă���
	img = pkl_dup(pkl);
	
	//�I���W�i������֊s���o
	pkl_rinkaku(pkl, factor);
	
	//���x���␳�őN�₩���𑝂�(�{���͍ʓx��������H)
	pkl_level(img, 1.5, 1.5, 1.0);
	
	//�g�U����
	if(bv1>1)
		pkl_noise(img, bv1);
	if(bv2>1)
		pkl_noise(img, bv2);
	
	//����
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
	
	//�O���[�C���[�W�����
	gray = malloc(pkl->width*pkl->height);
	if(!gray) return(1);
	
	/* convert grayscale */
	for(i=0; i<pkl->width*pkl->height; i++){
		store=0;
		for(k=0; k<pkl->channel; k++)
			store += pkl->image[i*pkl->channel+k];
		gray[i] = PKL_COLOR_CLIP(store/pkl->channel);
	}
	
	//�I���W�i���̃R�s�[�쐬
	g1 = pkl_dup(pkl);
	g2 = pkl_dup(pkl);
	
	//�R�s�[1�ɑ΂���
	//	�O���[�X�P�[�������֊s���o�����x���␳
	//pkl_gray(g1, PKL_GRAY_AVE);
		free(g1->image);
		g1->image = malloc(pkl->width*pkl->height);
		memcpy(g1->image, gray, pkl->width*pkl->height);
		g1->channel=1;
		g1->color=PKL_GRAYSCALE;
	//pkl_edge(g1, 10);
	pkl_emboss2(g1, mask25, 5, 5, 1.0, 255);
	pkl_level(g1, 0.015, 0.015, 1.0);

	//�R�s�[2�΂���
	//	�֊s���o���O���[�X�P�[���������x���␳
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
	//�{���m�C����
	if(pkl_voronoi_zone(pkl, zone, NULL, 0)) return(1);
	
	//���x���␳
	if(pkl_level(pkl, low, high, coeff)) return(1);
	
	//�J���[�G���{�X
	if(pkl_color_emboss(pkl, mil, env)) return (1);
	
	return(0);
}

//=============================================================================
// pkl_alphaedge
//=============================================================================
PKLExport int pkl_alphaedge(PKLImage pkl, int level, double low, double high, double coeff, int threshold, int alpha)
{
	PKLImage img;
	
	//�|�X�^���C�Y���m�C�Y����
	if(pkl_posterize(pkl, level)) return(1);
	if(pkl_noisecut(pkl)) return(1);
	
	//�R�s�[
	img = pkl_dup(pkl);
	//�R�s�[�����x���␳
	if(pkl_level(img, low, high, coeff)) return(1);
	
	//�I���W�i����P���J���[�A�b�v
	if(pkl_brightness(pkl, 50)) return(1);
	//�֊s���o
	if(pkl_edge(pkl, threshold)){
		pkl_close(img);
		return(1);
	}

	//����
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
