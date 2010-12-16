#include "pikl_scrap.h"

static unsigned char lighting(unsigned char color, double dx, double dy, double dz, double mil, double env, double *light);

//=============================================================================
// pkl_rgb2hsb(色相:Hue/彩度:Saturation/明度:Brightness)
//=============================================================================
static float *pkl_rgb2hsb(unsigned char *rgb, float *hsb)
{
	#define _max(a,b) (a>b) ? a : b
	#define _min(a,b) (a>b) ? b : a

	float hue, saturation, brightness;
	unsigned char color_max, color_min;
	unsigned char r, g, b;
	int diff;
	
	r = rgb[0];
	g = rgb[1];
	b = rgb[2];
	
	color_max= _max(_max(r, g), b);
	color_min= _min(_min(r, g), b);
	diff= color_max - color_min;
	
	hue=0.0;
	if(diff != 0){
		if(color_max==r) hue=60*(g-b)/(float)diff;
		if(color_max==g) hue=60*(b-r)/(float)diff + 120.0;
		if(color_max==b) hue=60*(r-g)/(float)diff + 240.0;
	}
	saturation = color_max!=0 ? diff/(float)color_max : 0.0;
	brightness = color_max / 255.0;

	hsb[0] = hue;
	hsb[1] = saturation;
	hsb[2] = brightness;

	return(hsb);
}

//=============================================================================
// pkl_hsb2rgb
//=============================================================================
static unsigned char *pkl_hsb2rgb(float *hsb, unsigned char *rgb)
{
	float h, s, b, f;
	int br, p, q, t, hue;

	h = hsb[0];
	s = hsb[1];
	b = hsb[2];
	
	h = (h>360.0) ? h-360.0 : (h<0.0) ? h+360.0 : h;
	s = (s>1.0) ? 1.0 : (s<0.0) ? 0.0 : s;
	br=(int)(255*b);
	br = PKL_COLOR_CLIP(br);

	if(s==0.0){
		rgb[0]=br;
		rgb[1]=br;
		rgb[2]=br;
	}else{
		hue=(int)(h/60.0);
		f=h/60.0-hue;
		
		p=(int)(br*(1.0-s));
		p = PKL_COLOR_CLIP(p);
		
		q=(int)(br*(1.0-f*s));
		q = PKL_COLOR_CLIP(q);
		
		t=(int)(br*(1.0-(1.0-f)*s));
		t = PKL_COLOR_CLIP(t);
		
		switch(hue){
			case 1:rgb[0]=q;  rgb[1]=br; rgb[2]=p; break;
			case 2:rgb[0]=p;  rgb[1]=br; rgb[2]=t; break;
			case 3:rgb[0]=p;  rgb[1]=q;  rgb[2]=br; break;
			case 4:rgb[0]=t;  rgb[1]=p;  rgb[2]=br; break;
			case 5:rgb[0]=br; rgb[1]=p;  rgb[2]=q; break;
			case 0:
			default:
				rgb[0]=br; rgb[1]=t; rgb[2]=p; break;
		}
	}
	return(rgb);
}

//=============================================================================
// pkl_sobelpaint
//=============================================================================
PKLExport int pkl_sobelpaint(PKLImage pkl, int edgeweight, float mix, float saturation, float hue)
{
	//Sobel Filter
	double Fx[3][3] = { {-0.25,   0.0,  0.25},
						{-0.50,   0.0,  0.50},
						{-0.25,   0.0,  0.25}};
	double Fy[3][3] = { {-0.25, -0.50, -0.25},
						{  0.0,   0.0,   0.0},
						{ 0.25,  0.50,  0.25}};
	double kg[3] = {0.298912, 0.586611, 0.114478};
	double dx, dy, dR, dG, dB, weight;
	int i, j, tx, ty, offset=1;
	unsigned char *wrk;
	float hsb[3], bright;

	if(pkl->color != PKL_RGB) return(1);
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0xff, pkl->width*pkl->height*pkl->channel);

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			if(j-offset<0 || j+offset>=pkl->width || i-offset<0 || i+offset>=pkl->height) continue;
			
			dR=dG=dB=0.0;
			for(ty=-offset; ty<=offset; ty++){
				for(tx=-offset; tx<=offset; tx++){
					//if(j+tx<0 || j+tx>=pkl->width || i+ty<0 || i+ty>=pkl->height) continue;
					weight = Fx[ty+offset][tx+offset];
					dR += (pkl->image[((i+ty)*pkl->width+j+tx)*pkl->channel+0] * weight);
					dG += (pkl->image[((i+ty)*pkl->width+j+tx)*pkl->channel+1] * weight);
					dB += (pkl->image[((i+ty)*pkl->width+j+tx)*pkl->channel+2] * weight);
				}
			}
			dx = kg[0]*dR + kg[1]*dG + kg[2]*dB;
			
			dR=dG=dB=0.0;
			for(ty=-offset; ty<=offset; ty++){
				for(tx=-offset; tx<=offset; tx++){
					//if(j+tx<0 || j+tx>=pkl->width || i+ty<0 || i+ty>=pkl->height) continue;
					weight = Fy[ty+offset][tx+offset];
					dR += (pkl->image[((i+ty)*pkl->width+j+tx)*pkl->channel+0] * weight);
					dG += (pkl->image[((i+ty)*pkl->width+j+tx)*pkl->channel+1] * weight);
					dB += (pkl->image[((i+ty)*pkl->width+j+tx)*pkl->channel+2] * weight);
				}
			}
			dy = kg[0]*dR + kg[1]*dG + kg[2]*dB;
			
			//RGB->HSB
			pkl_rgb2hsb(&pkl->image[(i*pkl->width+j)*pkl->channel], hsb);

			//明度にのみsobelを適用して元明度と合成する
			bright = hsb[2];
			hsb[2] = sqrt(dx*dx+dy*dy) * edgeweight * 0.01;
			hsb[2] = (hsb[2]>1.0) ? 0.0 : 1.0-hsb[2];
			hsb[2] = hsb[2]*(1.0-mix) + bright*mix;

			//彩度調整
			hsb[1] *= saturation;
			
			//色相調整
			hsb[0] += hue;
			
			//HSB->RGB
			pkl_hsb2rgb(hsb, &wrk[(i*pkl->width+j)*pkl->channel]);
		}
	}
	free(pkl->image);
	pkl->image=wrk;
	return(0);
}

//=============================================================================
// pkl_illust
//=============================================================================
PKLExport int pkl_illust(PKLImage pkl, int gap, int edge, int gammaint)
{
	int sobel1[9] = { 1, 0, -1, 2, 0, -2,  1,  0, -1 };	//sobel filter
	int sobel2[9] = { 1, 2,  1, 0, 0,  0, -1, -2, -1 };	//sobel filter
	int i, j, k, s, t, px, py, rp, r1, r2, gt;
	int wx[PKL_CHANNEL], wy[PKL_CHANNEL], rate[PKL_CHANNEL];
	unsigned char *wrk;
	double gamma;
	int th1, th2, th3;

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	
	gamma = 1.0/((double)gammaint/100.0);
	th1 = (int)(pow(128.0/255.0, gamma)*255.0);
	th2 = (int)(pow( 96.0/255.0, gamma)*255.0);
	th3 = (int)(pow( 64.0/255.0, gamma)*255.0);
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			memset(wx, 0, sizeof(wx));
			memset(wy, 0, sizeof(wy));
			
			for(s=0; s<3; s++){
				for(t=0; t<3; t++){
					px = (j+t-1)<0 ? 0 : (j+t-1)>=pkl->width ? pkl->width-1 : (j+t-1);
					py = (i+s-1)<0 ? 0 : (i+s-1)>=pkl->height ? pkl->height-1 : (i+s-1);
					for(k=0; k<pkl->channel; k++){
						wx[k] += pkl->image[(py*pkl->width+px)*pkl->channel+k] * sobel1[s*3+t];
						wy[k] += pkl->image[(py*pkl->width+px)*pkl->channel+k] * sobel2[s*3+t];
					}
				}
			}
			
			for(k=0; k<pkl->channel; k++)
				rate[k] = (int)(sqrt((double)(wx[k]*wx[k]+wy[k]*wy[k]))/8.0);
			rp = 0;
			for(k=0; k<pkl->channel; k++)
				if(rate[k]>rp) rp=rate[k];
			r1 = (rp<edge) ? 255 : (rp<edge+gap) ? 255-255*(rp-edge)/gap : 0;

			gt = 0;
			for(k=0; k<pkl->channel; k++)
				gt += pkl->image[(i*pkl->width+j)*pkl->channel+k];
			gt /= pkl->channel;
			r2 = (gt>th1) ? 255 : (gt>th2) ? 128 : (gt>th3) ? 64 : 0;
			
			for(k=0; k<pkl->channel; k++)
				wrk[(i*pkl->width+j)*pkl->channel+k] = PKL_COLOR_CLIP(pkl->image[(i*pkl->width+j)*pkl->channel+k] * r1/256*r2/256);
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_color_emboss
//=============================================================================
PKLExport int pkl_color_emboss(PKLImage pkl, double mil, double env)
{
	int sobel1[9] = { 1, 0, -1, 2, 0, -2,  1,  0, -1 };	//sobel filter
	int sobel2[9] = { 1, 2,  1, 0, 0,  0, -1, -2, -1 };	//sobel filter
	int i, j, k, s, t, wx, wy, gt, px, py;
	unsigned char *wrk, color[PKL_CHANNEL];
	double dx, dy, dz, light[3], abl;

	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);
	
	//光源ベクトルを単位ベクトルに補正する
	abl = sqrt(-1.0*-1.0 + -1.0*-1.0 + 1.0*1.0);
	light[0] = -1.0/abl;
	light[1] = -1.0/abl;
	light[2] =  1.0/abl;

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			wx=wy=0;
			for(s=0; s<3; s++){
				for(t=0; t<3; t++){
					px = (j+t-1)<0 ? 0 : (j+t-1)>=pkl->width ? pkl->width-1 : (j+t-1);
					py = (i+s-1)<0 ? 0 : (i+s-1)>=pkl->height ? pkl->height-1 : (i+s-1);
					
					gt=0;
					for(k=0; k<pkl->channel; k++)
						gt += pkl->image[(py*pkl->width+px)*pkl->channel+k];
					gt /= pkl->channel;
					wx += gt*sobel1[s*3+t];
					wy += gt*sobel2[s*3+t];
				}
			}
			dx = (double)wx;
			dy = (double)wy;
			dz = 16.0;
			for(k=0; k<pkl->channel; k++)
				color[k] = lighting(pkl->image[(py*pkl->width+px)*pkl->channel+k], dx, dy, dz, mil, env, light);
			memcpy(&wrk[(i*pkl->width+j)*pkl->channel], color, pkl->channel);
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	
	return(0);
}

//=============================================================================
// 陰影付け
// (dx,dy,dz)を法線ベクトルとする（単位ベクトルでなくてもよい）
//=============================================================================
static unsigned char lighting(unsigned char color, double dx, double dy, double dz,
                                                    double mil, double env, double *light)
{
	double abl, rz, env2, cosT, cosT2;
	int res;

	//法線ベクトルを単位ベクトルに補正
	abl = sqrt(dx*dx + dy*dy + dz*dz);
	cosT = (dx/abl)*light[0] + (dy/abl)*light[1] + (dz/abl)*light[2];
	rz = 2.0*cosT*(dz/abl) - light[2];
	cosT2 = (rz>0.0) ? pow(rz, 12.0) : 0.0;
	cosT  = (cosT<0.0) ? 0.0 : cosT;
	
	env2=env*255.0/100.0;
	res=(int)((cosT*255.0+env2)*(double)color/255.0);
	
	if(res>color) res=color;
	res+=(mil*cosT2);
	
	return(PKL_COLOR_CLIP(res));
}

//=============================================================================
// pkl_swirl
//=============================================================================
PKLExport int pkl_swirl(PKLImage pkl, double factor, int x, int y, PKLColor backcolor, PKL_SAMPLE sample)
{
	unsigned char *wrk, color[PKL_CHANNEL];
	int i, j, tx, ty;
	double range, rad, fx, fy;
	void (*op)(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);
	
	if((op=sample_select(sample)) == NULL)
		return(1);
	
	if(fabs(factor) > 1) return(1);
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);
	
	if(backcolor)
		memcpy(color, backcolor->color, pkl->channel);
	else
		memset(color, 0xff, pkl->channel);
	
	//中心座標
	tx = (x<0 || x>=pkl->width) ? pkl->width/2 : x;
	ty = (y<0 || y>=pkl->height) ? pkl->height/2 : y;
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			range = sqrt((j-tx)*(j-tx) + (i-ty)*(i-ty));
			rad = atan2(i-ty, j-tx) + (factor * range);
			fx = range * cos(rad) + tx;
			fy = range * sin(rad) + ty;
			op(pkl, &wrk[(i*pkl->width+j)*pkl->channel], fx, fy, color);
		}
	}

	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=============================================================================
// pkl_wave
//=============================================================================
PKLExport int pkl_wave(PKLImage pkl, PKL_WAVE mode, double factor, double frequency, PKLColor backcolor, PKL_SAMPLE sample)
{
	unsigned char *wrk, color[PKL_CHANNEL];
	int i, j;
	double fx, fy;
	void (*op)(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);
	
	if((op=sample_select(sample)) == NULL)
		return(1);
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	memset(wrk, 0, pkl->width*pkl->height*pkl->channel);
	
	if(backcolor)
		memcpy(color, backcolor->color, pkl->channel);
	else
		memset(color, 0xff, pkl->channel);
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			fx = (double)j;
			fy = (double)i;
			
			switch(mode){
				case PKL_WAVE_HORZ:
					fy += factor * sin(frequency * M_PI/180.0 * j);
					break;
				case PKL_WAVE_VERT:
					fx += factor * sin(frequency * M_PI/180.0 * i);
					break;
				case PKL_WAVE_BOTH:
				default:
					fx += factor * sin(frequency * M_PI/180.0 * i);
					fy += factor * sin(frequency * M_PI/180.0 * j);
			}
			op(pkl, &wrk[(i*pkl->width+j)*pkl->channel], fx, fy, color);
		}
	}

	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=================================================================================
// pkl_cylinder
//=================================================================================
PKLExport int pkl_cylinder(PKLImage pkl, PKL_CYLINDER_TYPE type, PKL_SAMPLE sample)
{
	unsigned char *wrk, color[PKL_CHANNEL];
	int i, j;
	double half, s, a, fx, fy;
	PKL_CYLINDER_TYPE hz, vt;
	void (*op)(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);
	
	if((op=sample_select(sample)) == NULL)
		return(1);
	
	hz = (type==PKL_CYLINDER_HORZ || type==PKL_CYLINDER_BOTH) ? 1 : 0;
	vt = (type==PKL_CYLINDER_VERT || type==PKL_CYLINDER_BOTH) ? 1 : 0;
	if(!hz && !vt)
		return(1);
	
	memset(color, 0, PKL_CHANNEL);
	
	//horizon変形
	if(hz){
		wrk = malloc(pkl->width*pkl->height*pkl->channel);
		if(!wrk) return(1);
		memset(wrk, 0, pkl->width*pkl->height*pkl->channel);
	
		half = (double)pkl->height/2.0;
		for(i=0; i<pkl->height; i++){
			s = (double)i - half;
			a = atan2(sqrt(half*half-s*s), s);
			fy = 2.0 * half * (1.0 - a / M_PI);
			
			for(j=0; j<pkl->width; j++){
				fx = (double)j;
				op(pkl, &wrk[(i*pkl->width+j)*pkl->channel], fx, fy, color);
			}
		}
		free(pkl->image);
		pkl->image = wrk;
	}
	//vertical変形
	if(vt){
		wrk = malloc(pkl->width*pkl->height*pkl->channel);
		if(!wrk) return(1);
		memset(wrk, 0, pkl->width*pkl->height*pkl->channel);
		
		half = (double)pkl->width/2.0;
		for(j=0; j<pkl->width; j++){
			s = (double)j - half;
			a = atan2(sqrt(half*half-s*s), s);
			fx = 2.0 * half * (1.0 - a / M_PI);
			
			for(i=0; i<pkl->height; i++){
				fy = (double)i;
				op(pkl, &wrk[(i*pkl->width+j)*pkl->channel], fx, fy, color);
			}
		}
		free(pkl->image);
		pkl->image = wrk;
	}
	
	return(0);
}

//=============================================================================
// pkl_dots
//=============================================================================
PKLExport int pkl_dots(PKLImage pkl, int zone, PKLColor dotscolor, PKLColor backcolor)
{
	unsigned char *wrk, *image, *p;
	int i, j, k, t, s, gray, count, stock;
	double propc;
	struct _PKLColor dot;
	
	if(zone<3 || zone>pkl->width-1 || zone>pkl->height-1)
		return(1);
	
	wrk = malloc(pkl->width*pkl->height*pkl->channel);
	if(!wrk) return(1);
	
	//背景色
	if(backcolor)
		for(i=0; i<pkl->width*pkl->height*pkl->channel; i+=pkl->channel)
			memcpy(&wrk[i], backcolor->color, pkl->channel);
	else
		memset(wrk, 0xff, pkl->width*pkl->height*pkl->channel);
	
	//ドット色
	if(dotscolor)
		memcpy(&(dot.color), dotscolor->color, pkl->channel);
	else
		memset(&(dot.color), 0, pkl->channel);
	
	//propc = zone/32.0;
	propc = zone/28.5;
	
	image = pkl->image;
	pkl->image = wrk;
	
	for(i=0; i<pkl->height; i+=zone){
		for(j=0; j<pkl->width; j+=zone){
			count=0;
			stock=0;
			for(t=i; t<i+zone; t++){
				for(s=j; s<j+zone; s++){
					if(t>=pkl->height || s>=pkl->width) continue;
					p = &image[(t*pkl->width+s)*pkl->channel];
					for(k=0,gray=0; k<pkl->channel; k++)
						gray += *p++;
					stock += (255-gray/pkl->channel);
					count++;
				}
			}
			if(count)
				draw_circle(pkl, j+zone/2, i+zone/2, sqrt(stock/count)*propc, &dot);
		}
	}
	
	free(image);
	return(0);
}

//=================================================================================
// pkl_sphere
//=================================================================================
PKLExport int pkl_sphere(PKLImage pkl, PKLColor backcolor, PKL_SAMPLE sample)
{
	unsigned char *wrk, color[PKL_CHANNEL];
	int i, j, mx, my, tx, ty;
	double th, fx, fy, nr, radius;
	void (*op)(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);
	
	if((op=sample_select(sample)) == NULL)
		return(1);
	
	wrk = malloc(pkl->width * pkl->height * pkl->channel);
	if(!wrk) return(1);
	
	if(backcolor)
		memcpy(color, backcolor->color, pkl->channel);
	else
		memset(color, 0xff, pkl->channel);
	
	mx = pkl->width / 2;
	my = pkl->height / 2;

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			tx = j - mx;
			ty = i - my;
			
			if(tx == 0)
				th = (ty>=0) ? M_PI/2 : -(M_PI/2);
			else
				th = atan2(ty, tx);
			
			radius = sqrt(tx*tx + ty*ty);
			nr = radius*radius / PKL_MAX(mx, my);
			
			fx = mx + nr*cos(th);
			fy = my + nr*sin(th);
			op(pkl, &wrk[(i*pkl->width+j)*pkl->channel], fx, fy, color);
		}
	}
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=================================================================================
// pkl_fisheye
//=================================================================================
PKLExport int pkl_fisheye(PKLImage pkl, double weight, PKL_SAMPLE sample)
{
	unsigned char *wrk, color[PKL_CHANNEL];
	int i, j, mx, my, radius;
	double fx, fy, rp;
	void (*op)(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);
	
	if((op=sample_select(sample)) == NULL)
		return(1);
	
	radius = pkl->width<pkl->height ? pkl->width : pkl->height;
	
	memset(color, 0, PKL_CHANNEL);
	wrk = malloc(pkl->width * pkl->height * pkl->channel);
	if(!wrk) return(1);
	
	mx = pkl->width / 2;
	my = pkl->height / 2;
	
	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			rp = sqrt(weight*weight + (j-mx)*(j-mx) + (i-my)*(i-my));
			fx = rp*(j-mx)/radius + mx;
			fy = rp*(i-my)/radius + my;
			op(pkl, &wrk[(i*pkl->width+j)*pkl->channel], fx, fy, color);
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

//=================================================================================
// pkl_sinusoidal
//=================================================================================
PKLExport int pkl_sinusoidal(PKLImage pkl, double weight, PKL_SAMPLE sample)
{
	unsigned char *wrk, color[PKL_CHANNEL];
	int i, j;
	double fx, fy;
	void (*op)(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);
	
	if((op=sample_select(sample)) == NULL)
		return(1);
	
	memset(color, 0, PKL_CHANNEL);
	wrk = malloc(pkl->width * pkl->height * pkl->channel);
	if(!wrk) return(1);

	for(i=0; i<pkl->height; i++){
		for(j=0; j<pkl->width; j++){
			fx = weight * sin(j) + j;
			fy = weight * sin(i) + i;
			fx = fx<0 ? 0.0 : fx>pkl->width-1 ? pkl->width-1 : fx;
			fy = fy<0 ? 0.0 : fy>pkl->height-1 ? pkl->height-1 : fy;
			op(pkl, &wrk[(i*pkl->width+j)*pkl->channel], fx, fy, color);
		}
	}
	free(pkl->image);
	pkl->image = wrk;
	return(0);
}

////=================================================================================
//// pkl_test
////=================================================================================
////Ripple:中心点を変えられるようにしたい！
//PKLExport int pkl_test(PKLImage pkl)
//{
//	unsigned char *wrk, color[PKL_CHANNEL];
//	int i, j;
//	double fx, fy, r, a;
//	
//	memset(color, 0, PKL_CHANNEL);
//	wrk = malloc(pkl->width * pkl->height * pkl->channel);
//	if(!wrk) return(1);
//
//	for(i=0; i<pkl->height; i++){
//		for(j=0; j<pkl->width; j++){
//			
//			//r  = sqrt(j*j + i*i);
//			//a  = atan2(j, i);
//
//			r  = sqrt((j+30)*(j+30) + (i+30)*(i+30));
//			a  = atan2((j+30), (i+30));
//
//
//			fx = -5.0 * sin(a) * cos(r) +j;
//			fy = -5.0 * cos(a) * sin(r) +i;
//
////printf("%f %f\n", fx, fy);
//
//			//pkl_nearestneighbor(pkl, &wrk[(i*pkl->width+j)*pkl->channel], fx, fy, color);
//			pkl_bilinear(pkl, &wrk[(i*pkl->width+j)*pkl->channel], fx, fy, color);
//		}
//	}
//
//	free(pkl->image);
//	pkl->image = wrk;
//	return(0);
//}
//


//uses
//  VCLImageUtils;
//
//function Edge2(var bmp: TBitmap):Boolean;
//var
//  bmp8:TBitmap;
//  w, h, ix, iy, i: integer;
//  d: TRGBTriple;
//  src: TBmpData24;
//  dst: TBmpData8;
//  ct: TPaletteEntries;
//
//  r1, r2, r3, rr, gg, bb: single;
//begin
//  result := false;
//  if bmp.PixelFormat ＜＞ pf24bit then exit;
//  w := bmp.Width;
//  h := bmp.Height;
//
//  bmp8 := TBitmap.Create;
//  bmp8.PixelFormat := pf8bit;
//  bmp8.Width := w;
//  bmp8.Height := h;
//  for i := 0 to 255 do
//  begin
//    ct[i].rgbBlue := i;
//    ct[i].rgbGreen := i;
//    ct[i].rgbRed := i;
//    ct[i].rgbReserved := 0;
//  end;
//  SetPalette8(bmp8, ct);
//
//  r3 := sqrt(3.0);
//
//  src := TBmpData24.Create(bmp);
//  dst := TBmpData8.Create(bmp8);
//
//  for iy := 0 to h-1 do
//    for ix := 0 to w-1 do
//    begin
//      d := src[ix,iy]^;
//
//      if ix+1＞w-1 then
//        r1 := 0
//      else
//      begin
//        bb := d.rgbtBlue - src[ix+1,iy]^.rgbtBlue;
//        gg := d.rgbtGreen - src[ix+1,iy]^.rgbtGreen;
//        rr := d.rgbtRed - src[ix+1,iy]^.rgbtRed;
//        r1 := trunc(sqrt(rr*rr+gg*gg+bb*bb)/r3);
//      end;
//
//      if iy+1＞h-1 then
//        r2 := 0
//      else
//      begin
//        bb := d.rgbtBlue - src[ix,iy+1]^.rgbtBlue;
//        gg := d.rgbtGreen - src[ix,iy+1]^.rgbtGreen;
//        rr := d.rgbtRed - src[ix,iy+1]^.rgbtRed;
//        r2 := trunc(sqrt(rr*rr+gg*gg+bb*bb)/r3);
//      end;
//
//      dst[ix,iy]^ := AdjustByte(255-r1-r2);
//    end;
//
//  dst.Free;
//  src.Free;
//  bmp.Free;
//  bmp := bmp8;
//  result := true;
//end;
//
