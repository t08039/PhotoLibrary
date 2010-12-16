#include "pikl_affine.h"

static point2 *transform(matrix *mat, int x, int y, point2 *p);
static matrix *pkl_matrix(PKLImage pkl, matrix *mat, double x_scale, double y_scale, double angle, int x, int y);
static void matrix_init(matrix *mat);
static void matrix_build(matrix *mat1, matrix *mat2);

//=============================================================================
// pkl_affine
//=============================================================================
PKLExport int pkl_affine(PKLImage pkl, PKL_SAMPLE sample, int width, int height, PKLColor backcolor,
                                               double x_scale, double y_scale, double angle, int x, int y)
{
	unsigned char *wrk, back[PKL_CHANNEL];
	int i, j;
	point2 p;
	matrix mat;
	void (*op)(PKLImage pkl, unsigned char *dst, double fx, double fy, unsigned char *color);
	
	if((op=sample_select(sample)) == NULL)
		return(1);
	
	//affine-matrix‚Ì\’z
	pkl_matrix(pkl, &mat, x_scale, y_scale, angle, x, y);
	
	wrk = malloc(width * height * pkl->channel);
	if(!wrk) return(1);
	
	//”wŒiF‚ðì‚é
	if(backcolor)
		memcpy(back, backcolor->color, PKL_CHANNEL);
	else
		memset(back, 0xff, PKL_CHANNEL);
	
	for(i=0; i<height; i++){
		for(j=0; j<width; j++){
			transform(&mat, j, i, &p);
			op(pkl, &wrk[(i*width+j)*pkl->channel], p.x, p.y, back);
		}
	}
	
	free(pkl->image);
	pkl->image = wrk;
	pkl->width = width;
	pkl->height = height;
	return(0);
}

//=============================================================================
// transform
//=============================================================================
static point2 *transform(matrix *mat, int x, int y, point2 *p)
{
	p->x = mat->a*x + mat->c*y + mat->e;
	p->y = mat->b*x + mat->d*y + mat->f;
	return p;
}

//=============================================================================
// pkl_matrix
//=============================================================================
static matrix *pkl_matrix(PKLImage pkl, matrix *mat, double x_scale, double y_scale, double angle, int x, int y)
{
	matrix mat2;
	double c, s;
	
	matrix_init(mat);
	
	//scale
	matrix_init(&mat2);
	mat2.a = x_scale;
	mat2.d = y_scale;
	matrix_build(mat, &mat2);
	
	//rotate
	matrix_init(&mat2);
	c = cos(angle*3.14/180.0);
	s = sin(angle*3.14/180.0);
	mat2.a = c;
	mat2.b = s;
	mat2.c = -s;
	mat2.d = c;
	matrix_build(mat, &mat2);
	
	//translate
	matrix_init(&mat2);
	mat2.e = x;
	mat2.f = y;
	matrix_build(mat, &mat2);
	
	return(mat);
}

//=============================================================================
// matrix_init
//=============================================================================
static void matrix_init(matrix *mat)
{
	mat->a = 1.0;
	mat->b = 0.0;
	mat->c = 0.0;
	mat->d = 1.0;
	mat->e = 0.0;
	mat->f = 0.0;
}

//=============================================================================
// matrix_init
//=============================================================================
static void matrix_build(matrix *mat1, matrix *mat2)
{
	matrix mat;
	memcpy(&mat, mat1, sizeof(matrix));
	
	mat1->a = mat.a * mat2->a + mat.b * mat2->c;
	mat1->b = mat.a * mat2->b + mat.b * mat2->d;
	mat1->c = mat.c * mat2->a + mat.d * mat2->c;
	mat1->d = mat.c * mat2->b + mat.d * mat2->d;
	mat1->e = mat.e * mat2->a + mat.f * mat2->c + mat2->e;
	mat1->f = mat.e * mat2->b + mat.f * mat2->d + mat2->f;
}

