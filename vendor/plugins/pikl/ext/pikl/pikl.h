//=============================================================================
// ImageLibrary 'pikl'
//   author: soe(soezimaster@gmail.com)
//   date: 2008.4
//   update: 2008.11
//   thanks.
//		MemoNyanDum(http://junki.lix.jp/)
//		C言語で実装する画像処理アルゴリズムのすべて-画像処理プログラミング
//		Sharaku Image Manipulation Program(http://www21.atwiki.jp/submarine/)
//		http://www.geocities.com/SiliconValley/Program/8979/color/index.htm
//		http://www.teu.ac.jp/clab/kondo/research/jimmy
//		http://www.sm.rim.or.jp/~shishido
//		adobe gil
//		Ryota Maruko
//=============================================================================
#ifndef _PIKL_
#define _PIKL_

#ifdef WIN32
	#define PKLExport __declspec(dllexport)
	#define XMD_H
#else
	#define PKLExport
#endif

#if defined __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define PKL_VERSION "libpikl 3.0.9"

// PKL object
typedef struct _PKLImage * PKLImage;
typedef struct _PKLColor * PKLColor;

//---------------------------------------------------------------------------------------
// support format & color
//	JPEG:
//		gray/rgb/cmyk
//		* color-typeのYCbCrおよびYCCには読み取りも保存も対応していない
//	PNG:
//		gray/gray-alpha/rgb/rgb-alpha/palette
//		* alphaは読取は可能だが、保存はできない
//		* paletteはRGBとして扱われる
//	BMP(windows bitmap):
//		24bit and non-compress only
//---------------------------------------------------------------------------------------
typedef enum {
	PKL_FORMAT_ERROR,	// error
	PKL_FORMAT_JPEG,	// jpeg
	PKL_FORMAT_PNG,		// png
	PKL_FORMAT_BITMAP	// windows bitmap
} PKL_FORMAT;

typedef enum {
	PKL_UNKNOWN,		// error
	PKL_BLACKWHITE,		// non support
	PKL_GRAYSCALE,		// gray scale
	PKL_RGB,			// 24bit RGB
	PKL_RGBA,			// not support!
	PKL_YCbCr,			// not support!
	PKL_CMYK,			// CMYK(for jpeg)
	PKL_YCCK			// non support
} PKL_COLOR_SPACE;

//---------------------------------------------------------------------------------------
// サンプリングパターン
//	* 全てのサンプリングパターンで拡大縮小対応
//	* 但し、pixel-averageで拡大をする場合は、lanczosと品質は同じ
//---------------------------------------------------------------------------------------
typedef enum {
	PKL_SAMPLE_ERROR,
	PKL_SAMPLE_NN,	// nearest neighbor
	PKL_SAMPLE_BL,	// bilinear
	PKL_SAMPLE_BC,	// bicubic
	PKL_SAMPLE_PA,	// pixel-average
	PKL_SAMPLE_LZ	// lanczos
} PKL_SAMPLE;

//---------------------------------------------------------------------------------------
// 表色系定義
//---------------------------------------------------------------------------------------
typedef enum {
	PKL_GRAY,
	PKL_RED,
	PKL_GREEN,
	PKL_BLUE,
	PKL_CYAN,
	PKL_MAGENTA,
	PKL_YELLOW,
	PKL_BLACK
} PKL_COLOR_MODEL;

/** macros **/
//#define pkl_color(a,b,c,d) ((a<<24)+(b<<16)+(c<<8)+d)

/*************************************************************/
/** utility                                                 **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// 表色系に応じたカラーオブジェクトを作る
//	_gray,_rgb,_cmykはpkl_color_createのエイリアスです
//---------------------------------------------------------------------------------------
PKLExport PKLColor pkl_color_gray(unsigned char gray);
PKLExport PKLColor pkl_color_rgb(unsigned char red, unsigned char green, unsigned char blue);
PKLExport PKLColor pkl_color_cmyk(unsigned char c, unsigned char m, unsigned char y, unsigned char k);
PKLExport PKLColor pkl_color_create(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

//---------------------------------------------------------------------------------------
// ターゲットピクセルの表色系に応じた色情報を取得・設定する
//	指定したPKL_COLOR_MODELとイメージのカラースペースが一致しない場合は実行されません
//---------------------------------------------------------------------------------------
PKLExport unsigned char pkl_get_color(PKLImage pkl, int x, int y, PKL_COLOR_MODEL target);
PKLExport int pkl_set_color(PKLImage pkl, int x, int y, PKL_COLOR_MODEL target, unsigned char color);

//---------------------------------------------------------------------------------------
// ターゲットピクセルの色情報を取得・設定する
//	指定したPKL_COLOR_MODELとイメージのカラースペースが一致しない場合は実行されません
//	* pkl_pixel_getによって取得したPKLColorはpkl_color_closeしてください
//---------------------------------------------------------------------------------------
PKLExport PKLColor pkl_get_pixel(PKLImage pkl, int x, int y);
PKLExport int pkl_set_pixel(PKLImage pkl, int x, int y, PKLColor color);

//---------------------------------------------------------------------------------------
// PKLColorから各色情報を取り出します
//---------------------------------------------------------------------------------------
PKLExport unsigned char pkl_color(PKLColor color, PKL_COLOR_MODEL target);

//---------------------------------------------------------------------------------------
// PKLColorオブジェクトの破棄
//---------------------------------------------------------------------------------------
PKLExport void pkl_color_close(PKLColor color);


/*************************************************************/
/** basic-method                                            **/
/*************************************************************/
// PKLImageを構築
PKLExport PKLImage pkl_open(const char *in);

//open済みファイルディスクリプタを指定
PKLExport PKLImage pkl_fdopen(FILE *in);

// 保存
PKLExport int pkl_save(PKLImage pkl, const char *out, PKL_FORMAT format);

//open済みファイルディスクリプタを指定
PKLExport int pkl_output(PKLImage pkl, FILE *out, PKL_FORMAT format);

// PKLImageオブジェクトの破棄
PKLExport void pkl_close(PKLImage pkl);

// 画像フォーマット
PKLExport PKL_FORMAT pkl_format(PKLImage pkl);

// 画像の幅・高さ
PKLExport int pkl_width(PKLImage pkl);
PKLExport int pkl_height(PKLImage pkl);

// 色数を数える
//	* 色数(CMYKの場合はCMYのみカウント)
PKLExport int pkl_count(PKLImage pkl);

// カラースペースを返す
PKLExport PKL_COLOR_SPACE pkl_colorspace(PKLImage pkl);

// PNG/JPEG保存時の圧縮率を指示する
//	level: 0(無圧縮) .. 10(最高圧縮)。
//		指定がない場合は、デフォルトレベルが設定される
PKLExport int pkl_compress(PKLImage pkl, int level);

// 新規のキャンバスを作る
//	backcolor: NULLの時は白
PKLExport PKLImage pkl_canvas(int width, int height, PKL_COLOR_SPACE color, PKLColor backcolor);

// PKLImageの複製
PKLExport PKLImage pkl_dup(PKLImage pkl);


//---------------------------
// 解像度
//---------------------------
typedef enum {
	PKL_RESOLUTION_HORZ,	//水平方向
	PKL_RESOLUTION_VERT,	//垂直方向
	PKL_RESOLUTION_BOTH		//両方
} PKL_RESOLUTION_TYPE;

//解像度を取得＆設定
PKLExport double pkl_get_dpi(PKLImage pkl, PKL_RESOLUTION_TYPE res);
PKLExport int pkl_set_dpi(PKLImage pkl, PKL_RESOLUTION_TYPE res, double density);


/*************************************************************/
/** basic-operation                                         **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// トリミング
//	・パラメータは左上原点です
//	・左上原点はsx=0, sy=0になります。
//	sx: 切り出し位置(x)
//	sy: 切り出し位置(y)
//	width: sxからの切り出す幅(ピクセル数)
//	height: syからの切り出す高さ(ピクセル数)
//---------------------------------------------------------------------------------------
PKLExport int pkl_trim(PKLImage pkl, int sx, int sy, int width, int height);

//---------------------------------------------------------------------------------------
// 拡大・縮小
//	width: 横方向の変形後サイズ(ピクセル数)
//	height: 縦方向の変形後サイズ(ピクセル数)
//	sample: サンプリングパターン(PKL_SAMPLE参照)
//---------------------------------------------------------------------------------------
PKLExport int pkl_resize(PKLImage pkl, int width, int height, PKL_SAMPLE sample);

//---------------------------------------------------------------------------------------
// 回転
//	angle: 右回転角度
//	sample: サンプリングパターン(PKL_SAMPLE参照)
//	backcolor: 背景色(NULLの時は白)
//---------------------------------------------------------------------------------------
PKLExport int pkl_rotate(PKLImage pkl, double angle, PKL_SAMPLE sample, PKLColor backcolor);

//---------------------------------------------------------------------------------------
// affine変換(from adobe gil)
//	width, height: affine変換後のサイズ
//	backcolor: 背景色(NULLの時は白)
//	*_scale: 変形率の逆数
//	angle  : 左回りの回転角度
//	x, y: 移動方向。正数は上、左方向への移動
//---------------------------------------------------------------------------------------
PKLExport int pkl_affine(PKLImage pkl, PKL_SAMPLE sample, int width, int height, PKLColor backcolor,
                                               double x_scale, double y_scale, double angle, int x, int y);


/*************************************************************/
/** enhance(一般的なフィルタ)                               **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// アンシャープマスク
//	threshold:
//		適応量(0-255)
//		0の時：変化しません。
//		255の時：全ての色にアンシャープ処理が働きます
//	edge:
//		エッジ。想定結果が得られる範囲は-10 .. 10程度です。
//		edge=0の時：変化しません。
//		edge>0の時：値が大きいほど先鋭化されます。
//		edge<0の時：値が小さいほどぼやけます。
//		
//		想定結果が得られる範囲は-10 .. 10程度です。
//		これを超える場合は、品質の保証はありません。
//		※画質によりこの通りではありません。-10..10の範囲より小さくなる可能性があります。
//---------------------------------------------------------------------------------------
PKLExport int pkl_unsharp(PKLImage pkl, int threshold, double edge);

//---------------------------------------------------------------------------------------
// コントラスト調整
//	rate: 調整係数(-127 - 127)
//		0の時：変化しません。
//		* rateが0以上の時は周波数によるコントラスト強調がおこなわれます。
//		* rateが0未満の時は直線的に平坦化されます。
//---------------------------------------------------------------------------------------
PKLExport int pkl_contrast(PKLImage pkl, int rate);

//---------------------------------------------------------------------------------------
// レベル補正
//	ヒストグラムの平坦化をおこないます。各色の分布に偏りがある画像に有効な場合があります。
//	low: 全ピクセル数に対して、切り捨てる暗い色の総数の割合(0-100%)
//	high: 全ピクセル数に対して、切り捨てる明るい色の総数の割合(0-100%)
//	coeff: 平坦化時の係数。
//		1が標準です。1より小さい場合は暗く、1より大きい場合は明るくなります(0.0..2.0)
//---------------------------------------------------------------------------------------
PKLExport int pkl_level(PKLImage pkl, double low, double high, double coeff);

//---------------------------------------------------------------------------------------
// 明るさ調整
//	各色にcolor値を加算する単純な処理です。
//	color:
//		各色に加算する色。
//		255を指定すれば、ただの白い画像になります。
//		-255を指定すると、ただの黒い画像になります。
//---------------------------------------------------------------------------------------
PKLExport int pkl_brightness(PKLImage pkl, int color);

//---------------------------------------------------------------------------------------
// 輝度(明るさ)・彩度(鮮やかさ)・色相(色合い)調整
//	ym: 輝度(-1..1)。+0.1で10%up。0.0は変化なし
//	sm: 彩度(-1..1)。+0.1で10%up。0.0は変化なし
//	hd: 色相(360度回転)。360度の倍数では変化しません。
//		参考：R=113.2/Ye=173.0/G=225.0/Cy=293.2/B=353.0/Mg=45.0
//---------------------------------------------------------------------------------------
PKLExport int pkl_hls(PKLImage pkl, double ym, double sm, double hd);

//---------------------------------------------------------------------------------------
// ガンマ補正
//	gm: 補正係数(>=0.0)
//		gm<1.0：暗く調整される
//		gm=1.0：変化しない
//		gm>1.0：明るく調整される
//---------------------------------------------------------------------------------------
PKLExport int pkl_gamma(PKLImage pkl, double gm);

// ノイズ除去(median filter)
PKLExport int pkl_noisecut(PKLImage pkl);


/*************************************************************/
/** blur(ぼかし)                                            **/
/*************************************************************/
//---------------------------------------------------------------------------------------
//  (安易な)ぼかし
//	weight:	平均算出領域のサイズ(>0)
//			例えば、3を指定した場合、近傍3x3の領域の平均色が選択される
//			値が大きいほど、ぼかし度は強くなる。
// *weightを大きくしすぎると、処理が重くなる。注意せよ
//---------------------------------------------------------------------------------------
PKLExport int pkl_blur(PKLImage pkl, int weight);

//---------------------------------------------------------------------------------------
// ガウスぼかし
//	ガウス関数を使ったぼかし。「画像全体にぼかしが入る」「方向がない」
//		weight:	ぼかし強調度(>0.0)
//				値が大きいほど、ぼかし度は強くなる。
// *weightを大きくしすぎると、処理が重くなる。注意せよ
//---------------------------------------------------------------------------------------
PKLExport int pkl_gaussblur(PKLImage pkl, double weight);

//---------------------------------------------------------------------------------------
// 放射状ブラー(radiation)
//	x,y:	放射の中心座標(画像範囲内にない場合は中心)
//	ef:		中心からの強調度(>0.0。値が大きいほど、放射度が強くなる)
//	weight:	ぼかしの強調度(>0)
// *weightを大きくしすぎると、処理が重くなる。注意せよ
//---------------------------------------------------------------------------------------
PKLExport int pkl_rblur(PKLImage pkl, int x, int y, double ef, int weight);

//---------------------------------------------------------------------------------------
// 回転ブラー(angle)
//	x,y:	放射の中心座標(画像範囲内にない場合は中心)
//	ef:		中心からの強調度(>0.0。値が大きいほど、放射度が強くなる)
//	weight:	ぼかしの強調度(>0)
// *weightを大きくしすぎると、処理が重くなる。注意せよ
//---------------------------------------------------------------------------------------
PKLExport int pkl_ablur(PKLImage pkl, int x, int y, double ef, int weight);

//---------------------------------------------------------------------------------------
// うずまきブラー(whirlpool)
//	x,y:	放射の中心座標(画像範囲内にない場合は中心)
//	ef:		中心からの強調度(>0.0。値が大きいほど、放射度が強くなる)
//	weight:	ぼかしの強調度(>0)
//	angle:	回転角度(0は回転なし)
// *weightを大きくしすぎると、処理が重くなる。注意せよ
//---------------------------------------------------------------------------------------
PKLExport int pkl_wblur(PKLImage pkl, int x, int y, double ef, int weight, double angle);

//---------------------------------------------------------------------------------------
// モーションブラー(motion)
//	任意角度での直線ブラー
//		angle: ぼかす角度
//		range: ぼかし強調度
// *weightを大きくしすぎると、処理が重くなる。注意せよ
//---------------------------------------------------------------------------------------
PKLExport int pkl_mblur(PKLImage pkl, double angle, int weight);


/*************************************************************/
/** pattern                                                 **/
/*************************************************************/
//---------------------------------------------------------------------------------------
//  パターン描画
//	pw,ph:	パターンのサイズ
//			[六角形]phのサイズをpwの3倍に設定すると、縦横サイズが同じパターンになる
//			[ひし形]phのサイズをpwの2倍に設定すると、縦横サイズが同じパターンになる
//						ex1) pw=10,ph=10にすると横長のパターン
//						ex2) pw=10,ph=20にすると縦横同じ大きさのパターン
//						ex3) pw=10,ph=30にすると縦長のパターン
//			[  円  ]phのサイズをpwの3倍に設定すると、縦横サイズが同じパターンになる
//			[レンガ]phのサイズをpwの4倍に設定すると、縦横サイズが同じパターンになる
//	type:	塗り方式
//---------------------------------------------------------------------------------------
// パターンタイプ
typedef enum {
	PKL_PATTERN_HEXAGON,	//六角形(default)
	PKL_PATTERN_DIAMOND,	//ひし形
	PKL_PATTERN_CIRCLE,		//円
	PKL_PATTERN_BRICK		//レンガ
} PKL_PATTERN_TYPE;

// パターンの塗り方式
typedef enum {
	PKL_PAINT_LINE,	//ライン(パターン内の横方向の平均で塗る)(default)
	PKL_PAINT_AVE,	//モザイク(パターン内の平均色で塗る)
} PKL_PAINT_TYPE;

PKLExport int pkl_pattern(PKLImage pkl, int pw, int ph, PKL_PATTERN_TYPE mode, PKL_PAINT_TYPE type);

//---------------------------------------------------------------------------------------
//  モザイク処理
//	msx,msy:モザイクのピクセルサイズ(それぞれ、幅・高さ)
//	1以下の時には変化はありません。画像サイズ以上の時には１色の画像になります
//---------------------------------------------------------------------------------------
PKLExport int pkl_mosaic(PKLImage pkl, int msx, int msy);

//モザイクを円で表現
//	radius: 半径(>=3)
PKLExport int pkl_circle(PKLImage pkl, int radius);

//---------------------------------------------------------------------------------------
// モザイクグリッド
//	モザイク化した上でグリッド線を引く
//		msx,msy:	モザイクのピクセルサイズ(それぞれ、幅・高さ)
//		color:		グリッド線の調整値
//					グリッド線色は単色ではなく、モザイク領域の色を元に生成する
//					colorはモザイク色に加算されるだけ。
//						0の時：モザイクと同色になり表出しない
//					  255の時：いわゆる白線
//					 -255の時：いわゆる黒線
//---------------------------------------------------------------------------------------
PKLExport int pkl_grid(PKLImage pkl, int msx, int msy, int color);

// お風呂のタイル風
typedef enum {
	PKL_TILE_RECT,		//四角形
	PKL_TILE_HEXAGON,	//六角形
	PKL_TILE_DIAMOND,	//ひし形
	PKL_TILE_CIRCLE,	//円
	PKL_TILE_BRICK		//レンガ
} PKL_TILE_TYPE;
PKLExport int pkl_tile(PKLImage pkl, PKL_TILE_TYPE type, int msx, int msy, unsigned char level);



/*************************************************************/
/** ボロノイ分割                                            **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// ボロノイ分割(ゾーン指定)
//	zone: ボロノイサイズ(>=2, <w,h)
//	bordercolor: NULLの時は境界線を引かない。境界線色はpkl_color_createで作る
//	test: 真 AND bordercolor!=NULLの時、分割シュミレーション図を出力する
//---------------------------------------------------------------------------------------
PKLExport int pkl_voronoi_zone(PKLImage pkl, int zone, PKLColor bordercolor, int test);

//---------------------------------------------------------------------------------------
// ボロノイ分割(母点数指定)
//	count: 母点数(>=10)
//	bordercolor: NULLの時は境界線を引かない。境界線色はpkl_color_createで作る
//	test: 真 AND bordercolor!=NULLの時、分割シュミレーション図を出力する
//	
//	*zoneとの違いは、母点がよりランダムに配置されるという点
//---------------------------------------------------------------------------------------
PKLExport int pkl_voronoi_count(PKLImage pkl, int count, PKLColor bordercolor, int test);


/*************************************************************/
/** decrease                                                **/
/**   色数を変える(減色・表色系変更)                        **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// ポスタライズ(階調変更)
//	level:階調数(1-256) *256を何分割するかということ
//---------------------------------------------------------------------------------------
PKLExport int pkl_posterize(PKLImage pkl, int level);

//---------------------------------------------------------------------------------------
// 色数削減
//	単純にlevelで指定した下位ビットを削除する。
//	level: 無効にする下位ビット数(0-255)
//---------------------------------------------------------------------------------------
PKLExport int pkl_cutcolor(PKLImage pkl, int level);

// CMYK→RGB
PKLExport int pkl_rgb(PKLImage pkl);

// RGB→GRAYSCALE
typedef enum {
	PKL_GRAY_NRM,		//加重平均法(最も一般的なグレースケール)
	PKL_GRAY_MED,		//中間値法
	PKL_GRAY_AVE		//単純平均法
} PKL_GRAYTYPE;
PKLExport int pkl_gray(PKLImage pkl, PKL_GRAYTYPE type);

// CMYK/RGB/GRAYSCALE→2c
PKLExport int pkl_2c(PKLImage pkl, int threshold);


/*************************************************************/
/** divide(パターン分割)                                    **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// 格子のずらし配置
//	area:格子のサイズ
//	shift:格子をずらす範囲
//---------------------------------------------------------------------------------------
PKLExport int pkl_tileslit(PKLImage pkl, int area, int shift);

//---------------------------------------------------------------------------------------
// フォトフレーム分割
//	イメージを分割し、フォトフレームのようにする
//		backcolor:	背景色(NULLの時、白)
//		wbs,hbs:	オリジナルイメージの縦横分割数(>0)
//		margin:		キャンバスのマージン(上下左右の空間)(>0)
//		frame:		フレームの太さ(>=1)
//---------------------------------------------------------------------------------------
PKLExport int pkl_splitframe(PKLImage pkl, PKLColor backcolor, int wbs, int hbs, int margin, int frame);


/*************************************************************/
/** composite(合成)                                         **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// 画像の合成
//	parentの上にchildを乗せる
//		parent:		ベース(処理後はこのオブジェクトが更新される)
//		child:		乗せる画像(このオブジェクトは処理しても不可侵)
//		xpos,ypos:	childを乗せるparent上の座標.
//		transcolor:	透明にする色。NULLの時は透明色の設定なし
//	* parentとchildは同じカラーモードでなければエラーとなる
//---------------------------------------------------------------------------------------
PKLExport int pkl_composite(PKLImage parent, PKLImage child, int xpos, int ypos, PKLColor transcolor);

//---------------------------------------------------------------------------------------
// アルファブレンド
//	parentの上にchildをアルファブレンドで乗せる
//		parent: ベース(処理後はこのオブジェクトが更新される)
//		child: 乗せる画像(このオブジェクトは処理しても不可侵)
//		xpos,ypos：childを乗せるparent上の座標.
//		alpha: α値(0-255)
//				alpha<0の時: 特殊モード(画像の縁ほどアルファを強調する)
//				alpha=0の時:   childは100%で乗る
//				alpha=255の時：childは  0%で乗る
//	* parentとchildは同じカラーモードでなければエラーとなる
//---------------------------------------------------------------------------------------
PKLExport int pkl_alphablend(PKLImage parent, PKLImage child, int xpos, int ypos, int alpha);

//---------------------------------------------------------------------------------------
// 影付け
//	backcolor: NULLの時は白
//	shadowcolor: NULLの時は黒
//---------------------------------------------------------------------------------------
PKLExport int pkl_shadowframe(PKLImage pkl, int margin, PKLColor backcolor, PKLColor shadowcolor);

//---------------------------------------------------------------------------------------
// 任意の位置に影付きで画像を配置する
//	shadowcolor: NULLの時は黒
//---------------------------------------------------------------------------------------
PKLExport int pkl_randomshadow(PKLImage canvas, PKLImage pasteimage,
				int top, int left, int margin, PKLColor shadowcolor);


/*************************************************************/
/** effect(一般的なエフェクト)                              **/
/*************************************************************/
// 反転
PKLExport int pkl_invert(PKLImage pkl);

//---------------------------------------------------------------------------------------
// ソラリゼーション(invertの応用)
//	dark: 真の時、weightより大きい色を反転させる
//        偽の時、weightより小さい色を反転させる
//---------------------------------------------------------------------------------------
PKLExport int pkl_solarization(PKLImage pkl, int dark, unsigned char weight);

// 拡散
PKLExport int pkl_noise(PKLImage pkl, int noise);

//---------------------------------------------------------------------------------------
// (安易な)セピア
//	red_weight,green_weight,blue_weight:
//		red,green,blueそれぞれに対しての重み
//		値が大きいほど、その色が強く出る
//	* RGBのみに対応
//	* セピアっぽくするのであれば、redの重みを1.0として、他の重みを1.0より小さくする
//---------------------------------------------------------------------------------------
PKLExport int pkl_sepia(PKLImage pkl, double red_weight, double green_weight, double blue_weight);

//---------------------------------------------------------------------------------------
// (安易な)油絵化
//	油絵のようにする
//		weight:	筆の太さ(というか最頻色走査領域のサイズ)
//				例えば、3を指定した場合、近傍3x3の領域の最頻色が選択される
//	* weightを大きくしすぎると、処理が重くなる。注意せよ
//---------------------------------------------------------------------------------------
PKLExport int pkl_oilpaint(PKLImage pkl, int weight);

//---------------------------------------------------------------------------------------
// kuwahara(from MemoNyanDum)
//	kuwaharaオペレータを使った抽象化で油絵のようになる。
//	pkl_oilpaint()との違いは、エッジがより鮮明であるという点。
//		range: 調整領域(>=2)
//	* rangeを大きくしすぎると、処理が重くなる。注意せよ
//---------------------------------------------------------------------------------------
PKLExport int pkl_kuwahara(PKLImage pkl, int range);

//---------------------------------------------------------------------------------------
// VTR調
//	colspace;	行の間隔
//	gst:	横方向のブレ
//	cst:	行の色差分
//---------------------------------------------------------------------------------------
PKLExport int pkl_vtr(PKLImage pkl, int colspace, int gst, int cst);

//---------------------------------------------------------------------------------------
// dither
//---------------------------------------------------------------------------------------
typedef enum {
	PKL_DITHER_NON,				//pkl_dithercolorではdefaultとなる
	PKL_DITHER_FLOYDSTEINBERG,	//default
	PKL_DITHER_STUCCI,
	PKL_DITHER_SIERRA,
	PKL_DITHER_JAJUNI
} PKL_DITHER;
PKLExport int pkl_dither(PKLImage pkl, PKL_DITHER dither);

//---------------------------------------------------------------------------------------
// 輪郭抽出
//	edge: エッジの太さ
//---------------------------------------------------------------------------------------
PKLExport int pkl_edgepaint(PKLImage pkl, int edge);

//---------------------------------------------------------------------------------------
// 輪郭抽出
//	threshold: 輪郭強調度(値が小さいほど、輪郭判定は荒くなる-->全体が黒くなる)
//---------------------------------------------------------------------------------------
PKLExport int pkl_edge(PKLImage pkl, int threshold);

//---------------------------------------------------------------------------------------
// 輪郭抽出
//	輪郭を抽出して滑らかに描画する
//---------------------------------------------------------------------------------------
PKLExport int pkl_contour(PKLImage pkl);

//---------------------------------------------------------------------------------------
// エッジ(輪郭)を強調する
//	unsharpは画像全体をシャープにするが、これはエッジ部分のみの強調
//---------------------------------------------------------------------------------------
typedef enum {
	PKL_FOCUS_DETAIL,	//弱い
	PKL_FOCUS_FOCUS,	//中間
	PKL_FOCUS_EDGES		//強い
} PKL_FOCUS;
PKLExport int pkl_focus(PKLImage pkl, PKL_FOCUS type);

//---------------------------------------------------------------------------------------
// エンボス
//---------------------------------------------------------------------------------------
typedef enum {
	PKL_EMBOSS_EMBOSS,		//エンボス1
	PKL_EMBOSS_HEAVY,		//エンボス2
	PKL_EMBOSS_LIGHT,		//エンボス3
	PKL_EMBOSS_LAPLACIAN	//輪郭抽出
} PKL_EMBOSS;
PKLExport int pkl_emboss(PKLImage pkl, PKL_EMBOSS type);

//---------------------------------------------------------------------------------------
// カスタムテーブルを設定できるエンボス
//	mask: マスク配列
//	row: maskの1行の要素数
//	col: maskの行数
//	factor: 輪郭部分の協調度
//	offset: 輪郭以外の部分の加算色(127:グレー基調・255:白基調)
//---------------------------------------------------------------------------------------
PKLExport int pkl_emboss2(PKLImage pkl, int *mask, int row, int col, double factor, int offset);

//---------------------------------------------------------------------------------------
// 色の入れ替え(RGBのみ対応)
//---------------------------------------------------------------------------------------
typedef enum {
	PKL_RGB2RBG,
	PKL_RGB2BGR,
	PKL_RGB2BRG,
	PKL_RGB2GRB,
	PKL_RGB2GBR
} PKL_SWAPMODE;
PKLExport int pkl_swapcolor(PKLImage pkl, PKL_SWAPMODE mode);


/*************************************************************/
/** camera(カメラ撮影風)                                    **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// vignette
//	color: 透過色
//	trans: 透過率(0-100%)
//	radius: 半径
//	x, y: 中心点
//---------------------------------------------------------------------------------------
PKLExport int pkl_vignette(PKLImage pkl, PKLColor color, int trans, int radius, int x, int y);

// ラウンドぼかし(中心から離れるほどぼかしが強くなる)
PKLExport int pkl_vaseline(PKLImage pkl, int radius, int x, int y);

// vignette + vaseline
PKLExport int pkl_vv(PKLImage pkl, PKLColor color, int trans, int radius, int x, int y);

// 銀塩フィルム風
PKLExport int pkl_film(PKLImage pkl, PKLColor color, int trans);

//// スポットライト風
//PKLExport int pkl_spotlight(PKLImage pkl, int radius, PKLColor color, int trans);


/*************************************************************/
/** special(フィルターの組み合わせ)                         **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// ポスタライズ(階調変更)→ノイズ除去→レベル補正→輪郭抽出
//	level:階調数(1-256) *256を何分割するかということ
//	low,high,coeff:レベル補正の値(low=1.5%, high=1.5%, coeff=1.0)
//	edge: 0>= (0:エッジ線を引かない、1>:エッジ線を引く.指定値の線の太さのエッジを引く)
//---------------------------------------------------------------------------------------
PKLExport int pkl_edgeposter(PKLImage pkl, int level, double low, double high, double coeff, int edge);

//---------------------------------------------------------------------------------------
// クレヨン風
//	factor:輪郭部分の強調度(5～80)
//	bv1, bv2:拡散レベル(2-step)
//	alpha:合成割合
//---------------------------------------------------------------------------------------
PKLExport int pkl_crayon(PKLImage pkl, double factor, int bv1, int bv2, int alpha);

//---------------------------------------------------------------------------------------
// 輪郭
//---------------------------------------------------------------------------------------
PKLExport int pkl_rinkaku(PKLImage pkl, double factor);

//---------------------------------------------------------------------------------------
// あじさい風
//	zone: ボロノイサイズ(>=2, <w,h)
//	low,high,coeff:レベル補正の値(low=9.0%, high=9.0%, coeff=1.0)
//	mil,env:カラーエンボス値(mil=50, env=70)
//---------------------------------------------------------------------------------------
PKLExport int pkl_hydrangea(PKLImage pkl, int zone, double low, double high, double coeff, double mil, double env);

//---------------------------------------------------------------------------------------
// ふんわりエッジ
//	level:階調数(1-256) *256を何分割するかということ
//	low,high,coeff:レベル補正の値(low=1.5%, high=1.5%, coeff=1.0)
//	threshold:輪郭強調度
//	alpha:合成割合
//---------------------------------------------------------------------------------------
PKLExport int pkl_alphaedge(PKLImage pkl, int level, double low, double high, double coeff, int threshold, int alpha);

//---------------------------------------------------------------------------------------
// トイカメラ風
//	vv -> contrast -> blur
//---------------------------------------------------------------------------------------
PKLExport int pkl_toy(PKLImage pkl, PKLColor color, int trans, int radius, int x, int y, int rate, int weight);

/*************************************************************/
/** scrap(イマイチなシリーズ)                               **/
/*************************************************************/
//---------------------------------------------------------------------------------------
// 円筒変形
//---------------------------------------------------------------------------------------
typedef enum {
	PKL_CYLINDER_HORZ,	//横筒
	PKL_CYLINDER_VERT,	//縦筒
	PKL_CYLINDER_BOTH	//両方向
} PKL_CYLINDER_TYPE;
PKLExport int pkl_cylinder(PKLImage pkl, PKL_CYLINDER_TYPE type, PKL_SAMPLE sample);

//---------------------------------------------------------------------------------------
// 渦巻き
//	factor:	   渦巻き度合い(絶対値で1以下の範囲で指定する。0に近いほど、ゆるやかな渦巻きになる)
// 			     * 負数と正数の違いは回転方向
//	x,y:	   渦巻きの中心点(座標が画像内に収まらない場合は、中心となる)
//	backcolor: 背景色(NULLの時は白)
//	sample:    サンプリング
//---------------------------------------------------------------------------------------
PKLExport int pkl_swirl(PKLImage pkl, double factor, int x, int y, PKLColor backcolor, PKL_SAMPLE sample);

//---------------------------------------------------------------------------------------
// wave
//	factor:周期
//	frequency:振幅
//---------------------------------------------------------------------------------------
typedef enum {
	PKL_WAVE_HORZ,	//横方向のwave
	PKL_WAVE_VERT,	//縦方向のwave
	PKL_WAVE_BOTH	//縦横のwave
} PKL_WAVE;
PKLExport int pkl_wave(PKLImage pkl, PKL_WAVE mode, double factor, double frequency, PKLColor backcolor, PKL_SAMPLE sample);


//球状変形
PKLExport int pkl_sphere(PKLImage pkl, PKLColor backcolor, PKL_SAMPLE sample);

//魚眼レンズ変形
PKLExport int pkl_fisheye(PKLImage pkl, double weight, PKL_SAMPLE sample);

//---------------------------------------------------------------------------------------
// ドット絵
//	円の描画を自力でやっているので、微妙な仕上がり品。
//	zone:ドット化する領域の一辺のサイズ(>3)
//	dotscolor: ドットの色(NULLの時は黒)
//	backcolor: 背景色(NULLの時は白)
//---------------------------------------------------------------------------------------
PKLExport int pkl_dots(PKLImage pkl, int zone, PKLColor dotscolor, PKLColor backcolor);

//---------------------------------------------------------------------------------------
// カラーディザー
//	weight: 誤差点に確定しなかった色に対する加算色
//		+255以上の時は完全な白
//		-255以下の時は完全な黒
//---------------------------------------------------------------------------------------
PKLExport int pkl_colordither(PKLImage pkl, int weight);

//---------------------------------------------------------------------------------------
// fantasy風(from Jimmy Educational NPR system)
//	明度のみにsobelフィルタによる輪郭強調を施す
//		edgeweight: 輪郭強調度(0..5, 0:輪郭強調しない、5:輪郭最大強調)
//		mix: オリジナル画像の明度との合成割合(0..1, 1で100%オリジナルの明度を生かす=>輪郭強調されない)
//		saturation: 彩度調整(0..5, 0:最低(色がなくなる),1:彩度に調整をかけない,1>:彩度があがる)
//		hue: 色相(0..360)
//	* 輪郭抽出を明度に適用するため、黒(all=0)の場合には白とびします。
//---------------------------------------------------------------------------------------
PKLExport int pkl_sobelpaint(PKLImage pkl, int edgeweight, float mix, float saturation, float hue);

//---------------------------------------------------------------------------------------
// イラスト調
//	gap: エッジ判定の際の重み付け
//	edge: エッジの重み(0が最もエッジが強い。0..20程度が目安)
//	gammaint: 絵柄のガンマ評価値(0:暗く評価、100:明るく評価。0..100)
//---------------------------------------------------------------------------------------
PKLExport int pkl_illust(PKLImage pkl, int gap, int edge, int gammaint);

//---------------------------------------------------------------------------------------
// カラーエンボス
//	mil: ハイライト(1..400)
//	env: 環境光(1..100)
//---------------------------------------------------------------------------------------
PKLExport int pkl_color_emboss(PKLImage pkl, double mil, double env);


//正弦波様
PKLExport int pkl_sinusoidal(PKLImage pkl, double weight, PKL_SAMPLE sample);


//PKLExport int pkl_test(PKLImage pkl);


/*************************************************************/
/** drawing(test)                                           **/
/*************************************************************/
//円を描画する
PKLExport int draw_circle(PKLImage pkl, int x, int y, int radius, PKLColor color);

//アンチエイリアスに対応した円を描画する
PKLExport int draw_circle_aa(PKLImage pkl, int x, int y, int radius, PKLColor color, int aa);


#if defined __cplusplus
}
#endif

#endif
