#include "pikl_voronoi.h"

//=============================================================================
// voronoi_zone_query
//=============================================================================
static void voronoi_zone_query(PKLImage pkl, struct VoronoiPoint *vptr, int *id, int zone, int xzone, int yzone)
{
	struct VoronoiPoint *p;
	int i, j, k, tx, ty, xx, yy, minIx, minIy;
	double d, mind;
	
	//母点を生成
	#ifdef WIN32
	srand(1);
	#else
	srand48(1);
	#endif
	p = vptr;
	for(i=0; i<yzone; i++){
		for(j=0; j<xzone; j++){
			#ifdef WIN32
			tx = j * zone + rand()%zone;
			ty = i * zone + rand()%zone;
			#else
			tx = j * zone + lrand48()%zone;
			ty = i * zone + lrand48()%zone;
			#endif
			p->x = tx>=pkl->width ? pkl->width-1 : tx;
			p->y = ty>=pkl->height ? pkl->height-1 : ty;
			p++;
		}
	}
	
	for(i=0; i<pkl->height; i++){
		yy = i/zone;
		for(j=0; j<pkl->width; j++){
			xx = j/zone;
			mind=10000000;
			minIx=minIy=0;
			
			//カレントピクセルに最も近い母点を探す
			for(ty=yy-1; ty<=yy+1; ty++){
				if(ty<0 || ty>yzone-1) continue;
				for(tx=xx-1; tx<=xx+1; tx++){
					if(tx<0 || tx>xzone-1) continue;
					d = (vptr[ty*xzone+tx].x-j)*(vptr[ty*xzone+tx].x-j) +
						(vptr[ty*xzone+tx].y-i)*(vptr[ty*xzone+tx].y-i);
					if(d < mind){
						mind = d;
						minIx = tx;
						minIy = ty;
					}
				}
			}
			
			for(k=0; k<pkl->channel; k++)
				vptr[minIy*xzone+minIx].color[k] += pkl->image[(i*pkl->width+j)*pkl->channel+k];
			vptr[minIy*xzone+minIx].count++;

			//各ピクセルのボロノイIDをセット
			id[i*pkl->width+j] = minIy*xzone+minIx;
		}
	}
}

//=============================================================================
// pkl_voronoi_zone
//=============================================================================
PKLExport int pkl_voronoi_zone(PKLImage pkl, int zone, PKLColor bordercolor, int test)
{
	struct VoronoiPoint *vptr, *p;
	int xzone, yzone, *id;
	int i, j, k, tx, ty;
	
	if(zone < 2) return(1);
	if(zone>=pkl->width || zone>=pkl->height) return(1);
	xzone = pkl->width/zone;
	if((pkl->width%zone) != 0) xzone++;
	yzone = pkl->height/zone;
	if((pkl->height%zone) != 0) yzone++;
	
	//ボロノイ管理データ
	vptr = malloc(sizeof(struct VoronoiPoint) * xzone*yzone);
	memset(vptr, 0, sizeof(struct VoronoiPoint) * xzone*yzone);
	
	//各座標のボロノイIDを格納するデータ
	id = malloc(sizeof(int) * pkl->width*pkl->height);
	
	//ボロノイ管理データを作る
	voronoi_zone_query(pkl, vptr, id, zone, xzone, yzone);
	
	if(test){
		if(bordercolor){
			//分割シュミレーション(母点を置く)
			memset(pkl->image, 0xff, pkl->width*pkl->height*pkl->channel);
			for(i=0; i<xzone*yzone; i++)
				memcpy(&pkl->image[(vptr[i].y*pkl->width+vptr[i].x)*pkl->channel], bordercolor->color, pkl->channel);
		}
	}else{
		//各ボロノイポイントの平均色算出
		for(ty=0; ty<yzone; ty++){
			for(tx=0; tx<xzone; tx++){
				if(vptr[ty*xzone+tx].count == 0) continue;
				p = &vptr[ty*xzone+tx];
				for(k=0; k<pkl->channel; k++)
					p->ave[k] = PKL_COLOR_CLIP(p->color[k]/p->count);
			}
		}
		//色を設定
		for(i=0; i<pkl->height; i++)
			for(j=0; j<pkl->width; j++)
				memcpy(&pkl->image[(i*pkl->width+j)*pkl->channel], vptr[id[i*pkl->width+j]].ave, pkl->channel);
	}
	
	//境界線を引く
	if(bordercolor){
		int currentId, hit=0;
		for(j=0; j<pkl->height; j++){
			for(i=0; i<pkl->width; i++){
				hit=0;
				currentId = id[j*pkl->width+i];
				if(i-1>=0         && currentId!=id[j*pkl->width+i-1]) hit++;
				if(i+1<pkl->width && currentId!=id[j*pkl->width+i+1]) hit++;
				if(j-1>=0          && currentId!=id[(j-1)*pkl->width+i+1]) hit++;
				if(j+1<pkl->height && currentId!=id[(j+1)*pkl->width+i+1]) hit++;
				if(hit)
					memcpy(&pkl->image[(j*pkl->width+i)*pkl->channel], bordercolor->color, pkl->channel);
			}
		}
	}
	free(id);
	free(vptr);
	return(0);
}

//=============================================================================
// vcompare
//=============================================================================
static int vcompare(const void *a, const void *b)
{
	struct VORONOI *t1 = (struct VORONOI *)a;
	struct VORONOI *t2 = (struct VORONOI *)b;
	
	if(t1->y == t2->y)
		return(t1->x - t2->x);
	return(t1->y - t2->y);
}

//=============================================================================
// voronoi_setid
//=============================================================================
static void voronoi_setid(int *ary, int id)
{
	int i;
	
	for(i=0; i<VORONOI_CNT; i++){
		if(ary[i] == -1){
			ary[i] = id;
			return;
		}
	}
}

//=============================================================================
// voronoi_count_query
//=============================================================================
static void voronoi_count_query(PKLImage pkl, struct VORONOI *kp, int *ip, int count)
{
	int i, j, k, min_id, pre_id;
	double d, dx, dy, min_d;
	
	//母点をランダムに生成する
	#ifdef WIN32
	srand((long)count);
	#else
	srand48((long)count);
	#endif
	for(i=0; i<count; i++){
		#ifdef WIN32
		kp[i].x = rand() % pkl->width;
		kp[i].y = rand() % pkl->height;
		#else
		kp[i].x = lrand48() % pkl->width;
		kp[i].y = lrand48() % pkl->height;
		#endif
		kp[i].id = i;
	}
	
	//生成した母点座標を左上から右下に向かってソートする
	qsort(kp, count, sizeof(struct VORONOI), vcompare);
	
	min_id = 0;  //最初の母点ID
	pre_id = 0;  //前ピクセルの母点ID
	
	//各母点の最近傍の母点を探索する
	for(j=0; j<pkl->height; j++){
		for(i=0; i<pkl->width; i++){
			// 前ピクセルの母点との距離を求める
			dx = kp[pre_id].x-i;
			dy = kp[pre_id].y-j;
			min_d = sqrt(dx*dx+dy*dy);
			
			//現在のピクセルの右上により近い母点があるか検索
			for(k=pre_id-1; k>=0 && (kp[k].y>=j-min_d); k--){
				if(kp[k].x >= i && kp[k].x <= i+min_d){
					dx = kp[k].x-i;
					dy = kp[k].y-j;
					d = sqrt(dx*dx+dy*dy);
					if(d<min_d){
						min_id=k;
						min_d=d;
					}
				}
			}
			//現在のピクセルの右下により近い母点があるか検索
			for(k=pre_id+1; k<count && (kp[k].y<=j+min_d); k++){
				if(kp[k].x >= i && kp[k].x <= i+min_d){
					dx = kp[k].x-i;
					dy = kp[k].y-j;
					d = sqrt(dx*dx+dy*dy);
					if(d<min_d){
						min_id=k;
						min_d=d;
					}
				}
			}
			//最も近くにある母点のIDを設定する
			if(pre_id != min_id){
				voronoi_setid(kp[pre_id].next, min_id);
				voronoi_setid(kp[min_id].next, pre_id);
			}
			
			//現在の座標のIDに決定したボロノイ管理IDを設定する
			ip[j*pkl->width+i] = min_id;
			pre_id = min_id;
		}
	}
}

//=============================================================================
// pkl_voronoi_count
//=============================================================================
PKLExport int pkl_voronoi_count(PKLImage pkl, int count, PKLColor bordercolor, int test)
{
	struct VORONOI *kp;
	int i, j, k, *ip;

	//母点数の閾値
	if(count < 10) return(1);

	//各座標の母点IDを格納する領域
	ip = malloc(sizeof(int) * pkl->width * pkl->height);
	memset(ip, -1, sizeof(int) * pkl->width * pkl->height);
	
	//母点管理領域(母点数分用意)
	kp = malloc(sizeof(struct VORONOI) * count);
	memset(kp, 0, sizeof(struct VORONOI) * count);
	for(i=0; i<count; i++)
		memset(kp[i].next, -1, sizeof(int)*VORONOI_CNT);
	
	//ボロノイ管理データを作る
	voronoi_count_query(pkl, kp, ip, count);
	
	if(test){
		if(bordercolor){
			//分割シュミレーション(母点を置く)
			memset(pkl->image, 0xff, pkl->width*pkl->height*pkl->channel);
			for(i=0; i<count; i++)
				memcpy(&pkl->image[(kp[i].y*pkl->width+kp[i].x)*pkl->channel], bordercolor->color, pkl->channel);
		}
	}else{
		//ボロノイ分割されたので、各セルの平均色を算出するためにセルの色を累積する
		for(j=0; j<pkl->height; j++) {
			for(i=0; i<pkl->width; i++) {
				//ipを参照することで、現在の座標がどのIDに所属するかわかる
				if(ip[j*pkl->width+i] == -1) continue;
				//IDごとに色を累積する
				kp[ ip[j*pkl->width+i] ].count++;
				for(k=0; k<pkl->channel; k++)
					kp[ ip[j*pkl->width+i] ].color[k] += pkl->image[(j*pkl->width+i)*pkl->channel+k];
			}
		}
		
		//各セルの平均色を作る
		for(i=0; i<count; i++){
			for(k=0; k<pkl->channel; k++){
				if(kp[i].count!=0)
					kp[i].color[k] = PKL_COLOR_CLIP(kp[i].color[k]/kp[i].count);
			}
		}
		
		//色を置く
		for(j=0; j<pkl->height; j++)
			for(i=0; i<pkl->width; i++)
				for(k=0; k<pkl->channel; k++)
					pkl->image[(j*pkl->width+i)*pkl->channel+k] = kp[ ip[j*pkl->width+i] ].color[k];
	}
	
	//境界線を引く
	if(bordercolor){
		int currentId, hit=0;
		for(j=0; j<pkl->height; j++){
			for(i=0; i<pkl->width; i++){
				hit=0;
				currentId = ip[j*pkl->width+i];
				if(i-1>=0         && currentId!=ip[j*pkl->width+i-1]) hit++;
				if(i+1<pkl->width && currentId!=ip[j*pkl->width+i+1]) hit++;
				if(j-1>=0          && currentId!=ip[(j-1)*pkl->width+i+1]) hit++;
				if(j+1<pkl->height && currentId!=ip[(j+1)*pkl->width+i+1]) hit++;
				if(hit)
					memcpy(&pkl->image[(j*pkl->width+i)*pkl->channel], bordercolor->color, pkl->channel);
			}
		}
	}

	free(kp);
	free(ip);
	return(0);
}

