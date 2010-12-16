#include "pikl_rotate.h"

static int rotate_90(PKLImage pkl, double angle);
static int rotate_custom(PKLImage pkl, double angle, PKL_SAMPLE sample, PKLColor backcolor);
static void bsort(int *p);
static int rotate_operate(PKLImage pkl, PKL_SAMPLE sample, PIKL_ROTATE *ri);

//=================================================================================
// pkl_rotate
//=================================================================================
PKLExport int pkl_rotate(PKLImage pkl, double angle, PKL_SAMPLE sample, PKLColor backcolor)
{
	/* 90度の倍角かどうかで処理を分ける(誤差回避のため) */
	if((int)angle%90){
		return rotate_custom(pkl, angle, sample, backcolor);
	}else{
		return rotate_90(pkl, angle);
	}
	return(1);
}

//=============================================================================
// rotate_90
//=============================================================================
static int rotate_90(PKLImage pkl, double angle)
{
	PIKL_ROTATE ri;
	double c, s, rad;
	
	memset(&ri, 0, sizeof(PIKL_ROTATE));
	
	rad = -angle*M_PI/180.0;
	c = cos(rad);
	s = sin(rad);
	ri.rcos = (int)c==0 ? 0.0 : (int)c==1 ? 1.0 : -1.0;
	ri.rsin = (int)s==0 ? 0.0 : (int)s==1 ? 1.0 : -1.0;
	
	/*90*/
	if(ri.rcos==0 && ri.rsin==1){
		ri.width  = pkl->height;
		ri.height = pkl->width;
		ri.sx     = 0;
		ri.sy     = -(pkl->width-1);
		ri.ex     = pkl->height-1;
		ri.ey     = 0;
	}else
	/*180*/
	if(ri.rcos==-1 && ri.rsin==0){
		ri.width  = pkl->width;
		ri.height = pkl->height;
		ri.sx     = -(pkl->width-1);
		ri.sy     = -(pkl->height-1);
		ri.ex     = 0;
		ri.ey     = 0;
	}else
	/*270*/
	if(ri.rcos==0 && ri.rsin==-1){
		ri.width  = pkl->height;
		ri.height = pkl->width;
		ri.sx     = -(pkl->height-1);
		ri.sy     = 0;
		ri.ex     = 0;
		ri.ey     = pkl->width-1;
	}
	
	return rotate_operate(pkl, PKL_SAMPLE_NN, &ri);
}

//=============================================================================
// rotate_custom
//=============================================================================
static int rotate_custom(PKLImage pkl, double angle, PKL_SAMPLE sample, PKLColor backcolor)
{
	PIKL_ROTATE ri;
	double rad;
	int px[4], py[4], i, x, y;
	
	memset(&ri, 0, sizeof(PIKL_ROTATE));
	
	rad = -angle*M_PI/180.0;
	ri.rcos = cos(rad);
	ri.rsin = sin(rad);
	
	px[0] = 0;
	py[0] = 0;
	px[1] = pkl->width;
	py[1] = 0;
	px[2] = 0;
	py[2] = pkl->height;
	px[3] = pkl->width;
	py[3] = pkl->height;
	
	for(i=0; i<4; i++){
		x = (int)(cos(-rad)*px[i] - sin(-rad)*py[i]);
		y = (int)(sin(-rad)*px[i] + cos(-rad)*py[i]);
		px[i] = x;
		py[i] = y;
	}
	
	//昇順に並びかえ(qsortを使うと遅いのでバブルソートアルゴリズムで)
	bsort(px);
	bsort(py);
	ri.sx = px[0];
	ri.sy = py[0];
	ri.ex = px[3];
	ri.ey = py[3];
	ri.width = ri.ex - ri.sx + 1;
	ri.height = ri.ey - ri.sy + 1;
	
	//for(i=0; i<pkl->channel; i++)
	//	ri.back[i] = (backcolor>>((pkl->channel-i)*8) & 0xFF);
	if(backcolor)
		memcpy(ri.back, backcolor->color, PKL_CHANNEL);
	else
		memset(ri.back, 0xff, PKL_CHANNEL);

	return rotate_operate(pkl, sample, &ri);
}

//=============================================================================
// bsort
//=============================================================================
static void bsort(int *p)
{
	int i, j, k, t;
	
	k=3;
	while(k>=0){
		j=-1;
		for(i=1; i<=k; i++){
			if(p[i-1] > p[i]){
				j = i-1;
				t=p[j]; p[i-1]=p[i]; p[i]=t;
			}
		}
		k=j;
	}
}

//=================================================================================
// rotate_operate
//=================================================================================
static int rotate_operate(PKLImage pkl, PKL_SAMPLE sample, PIKL_ROTATE *ri)
{
	unsigned char *wrk;
	int i, j;
	double fx, fy;
	void (*op)(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);
	
	if((op=sample_select(sample)) == NULL)
		return(1);
	
	wrk = malloc(ri->width * ri->height * pkl->channel);
	if(!wrk) return(1);
	
	for(i=ri->sy; i<=ri->ey; i++){
		for(j=ri->sx; j<=ri->ex; j++){
			fx = ri->rcos*j - ri->rsin*i;
			fy = ri->rsin*j + ri->rcos*i;
			op(pkl, &wrk[((i-ri->sy)*ri->width+j-ri->sx)*pkl->channel], fx, fy, ri->back);
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	pkl->width = ri->width;
	pkl->height = ri->height;
	return(0);
}

