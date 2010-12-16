module Pikl
  require "dl/import"
  require "dl/struct"
  require "rbconfig"
  
  module Ext
    extend DL::Importable

    dlext = (RbConfig::CONFIG["host_os"] == 'mswin32') ? 'dll' : RbConfig::CONFIG['DLEXT']
    dlload "#{File.dirname(__FILE__)}/pikl.#{dlext}"

    typealias("FILE", "void")
    typealias("PKLImage", "void")
    typealias("PKLColor", "void")
    typealias("PKL_FORMAT", "int")
    typealias("PKL_ANGLE", "int")
    typealias("PKL_SAMPLE", "int")
    typealias("uchar", "unsigned char")
    typealias("PKL_COLOR_SPACE", "int")
    typealias("PKL_PATTERN_TYPE", "int")
    typealias("PKL_PAINT_TYPE", "int")
    typealias("PKL_DITHER", "int")
    typealias("PKL_WAVE", "int")
    typealias("PKL_FOCUS", "int")
    typealias("PKL_EMBOSS", "int")
    typealias("PKL_COLOR_MODEL", "int")
    typealias("PKL_TILE_TYPE", "int")
    typealias("PKL_GRAYTYPE", "int")
    typealias("PKL_RESOLUTION_TYPE", "int")

## utility
    extern "PKLColor *pkl_color_gray(uchar)"
    extern "PKLColor *pkl_color_rgb(uchar, uchar, uchar)"
    extern "PKLColor *pkl_color_cmyk(uchar, uchar, uchar, uchar)"
    extern "PKLColor *pkl_color_create(uchar, uchar, uchar, uchar)"
    extern "void pkl_color_close(PKLColor*)"
    extern "uchar pkl_get_color(PKLImage*, int, int, PKL_COLOR_MODEL)"
    extern "int pkl_set_color(PKLImage*, int, int, PKL_COLOR_MODEL, uchar)"
    extern "PKLColor *pkl_get_pixel(PKLImage*, int, int)"
    extern "int pkl_set_pixel(PKLImage*, int, int, PKLColor*)"
    extern "uchar pkl_color(PKLColor*, PKL_COLOR_MODEL)"
    extern "double pkl_get_dpi(PKLImage*, PKL_RESOLUTION_TYPE)"
    extern "int pkl_set_dpi(PKLImage*, PKL_RESOLUTION_TYPE, double)"

## basic method
    extern "PKLImage *pkl_open(char*)"
    extern "void pkl_close(PKLImage*)"
    extern "int pkl_save(PKLImage*, const char *, PKL_FORMAT)"
    extern "int pkl_compress(PKLImage*, int)"
    extern "PKLImage *pkl_canvas(int, int, PKL_COLOR_SPACE, PKLColor*)"
    extern "PKLImage *pkl_fdopen(FILE*)"
    extern "int pkl_output(PKLImage*, FILE*, PKL_FORMAT)"
    extern "PKLImage *pkl_dup(PKLImage*)"

## information
    extern "PKL_FORMAT pkl_format(PKLImage *)"
    extern "int pkl_width(PKLImage *)"
    extern "int pkl_height(PKLImage *)"
    extern "int pkl_count(PKLImage *)"
    extern "PKL_COLOR_SPACE pkl_colorspace(PKLImage*)"

## basic operation
    extern "int pkl_trim(PKLImage *, int, int, int, int)"
    extern "int pkl_resize(PKLImage*, int, int, PKL_SAMPLE)"
    extern "int pkl_rotate(PKLImage*, float, PKL_SAMPLE, PKLColor*)"
    extern "int pkl_affine(PKLImage*, PKL_SAMPLE, int, int, PKLColor*, double, double, double, int, int)"

## enhance
    extern "int pkl_unsharp(PKLImage *, int, double)"
    extern "int pkl_contrast(PKLImage*, int)"
    extern "int pkl_level(PKLImage *, double, double, double)"
    extern "int pkl_brightness(PKLImage*, int)"
    extern "int pkl_hls(PKLImage*, double, double, double)"
    extern "int pkl_gamma(PKLImage*, double)"
    extern "int pkl_noisecut(PKLImage*)"

## blur
    extern "int pkl_blur(PKLImage*, int)"
    extern "int pkl_gaussblur(PKLImage*, double)"
    extern "int pkl_rblur(PKLImage*, int, int, double, int)"
    extern "int pkl_ablur(PKLImage*, int, int, double, int)"
    extern "int pkl_wblur(PKLImage*, int, int, double, int, double)"
    extern "int pkl_mblur(PKLImage*, double, int)"

## pattern
    extern "int pkl_pattern(PKLImage*, int, int, PKL_PATTERN_TYPE, PKL_PAINT_TYPE)"
    extern "int pkl_mosaic(PKLImage*, int, int)"
    extern "int pkl_grid(PKLImage*, int, int, int)"
    extern "int pkl_tile(PKLImage*, PKL_TILE_TYPE, int, int, uchar)"

## voronoi
    extern "int pkl_voronoi_zone(PKLImage*, int, PKLColor*, int)"
    extern "int pkl_voronoi_count(PKLImage*, int, PKLColor*, int)"

## decrease
    extern "int pkl_posterize(PKLImage*, int)"
    extern "int pkl_cutcolor(PKLImage*, int)"
    extern "int pkl_rgb(PKLImage*)"
    extern "int pkl_gray(PKLImage*, PKL_GRAYTYPE)"
    extern "int pkl_2c(PKLImage*, int)"

## divide
    extern "int pkl_tileslit(PKLImage*, int, int)"
    extern "int pkl_splitframe(PKLImage*, PKLColor*, int, int, int, int)"

## composite
    extern "int pkl_composite(PKLImage*, PKLImage*, int, int, PKLColor*)"
    extern "int pkl_alphablend(PKLImage*, PKLImage*, int, int, int)"
    extern "int pkl_shadowframe(PKLImage*, int, PKLColor*, PKLColor*)"
    extern "int pkl_randomshadow(PKLImage*, PKLImage*, int, int, int, PKLColor*)"

## basic effect
    extern "int pkl_invert(PKLImage*)"
    extern "int pkl_sepia(PKLImage*, double, double, double)"
    extern "int pkl_oilpaint(PKLImage*, int)"
    extern "int pkl_kuwahara(PKLImage*, int)"
    extern "int pkl_noise(PKLImage*, int)"
    extern "int pkl_vtr(PKLImage*, int, int, int)"
    extern "int pkl_dither(PKLImage*, PKL_DITHER)"
    extern "int pkl_edgepaint(PKLImage*, int)"
    extern "int pkl_edge(PKLImage*, int)"
    extern "int pkl_contour(PKLImage*)"
    extern "int pkl_focus(PKLImage*, PKL_FOCUS)"
    extern "int pkl_emboss(PKLImage*, PKL_EMBOSS)"
    extern "int pkl_emboss2(PKLImage*, int *, int, int, double, int)"

## camera
    extern "int pkl_vignette(PKLImage*, PKLColor*, int, int, int, int)"
    extern "int pkl_vaseline(PKLImage*, int, int, int)"
    extern "int pkl_film(PKLImage*, PKLColor*, int)"
    extern "int pkl_vv(PKLImage*, PKLColor*, int, int, int, int)"

## special
    extern "int pkl_edgeposter(PKLImage*, int, double, double, double, int)"
    extern "int pkl_crayon(PKLImage*, double, int, int, int)"
    extern "int pkl_rinkaku(PKLImage*, double)"
    extern "int pkl_hydrangea(PKLImage*, int, double, double, double, double, double)"
    extern "int pkl_alphaedge(PKLImage*, int, double, double, double, int, int)"

## scrap
    extern "int pkl_swirl(PKLImage*, double, int, int, PKLColor*)"
    extern "int pkl_wave(PKLImage*, PKL_WAVE, double, double)"
    extern "int pkl_dots(PKLImage*, int, PKLColor*)"
    extern "int pkl_colordither(PKLImage*, int)"
    extern "int pkl_sobelpaint(PKLImage*, int, float, float, float)"
    extern "int pkl_illust(PKLImage*, int, int, int)"
    extern "int pkl_color_emboss(PKLImage*, double, double)"

  end
end
