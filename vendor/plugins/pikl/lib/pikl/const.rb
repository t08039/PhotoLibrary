module Pikl

  JPEG   = 1
  JPG    = JPEG
  PNG    = 2
  BITMAP = 3
  BMP    = BITMAP
  
  ANGLE000 = 0
  ANGLE090 = 1
  ANGLE180 = 2
  ANGLE270 = 3

  SAMPLE_NN	= 1  #nearest neighbor
  SAMPLE_BL	= 2  #bilinear
  SAMPLE_BC = 3  #bicubic
  SAMPLE_PA	= 4  #pixcel average(reduce only)
  SAMPLE_LZ	= 5  #lanczos

  SAMPLES = {
    :nearest_neighbor => SAMPLE_NN,
    :bilinear => SAMPLE_BL,
    :bicubic => SAMPLE_BC,
    :pixcel_averate => SAMPLE_PA,
    :lanczos => SAMPLE_LZ,
    :nn => SAMPLE_NN,
    :bl => SAMPLE_BL,
    :bc => SAMPLE_BC,
    :pa => SAMPLE_PA,
    :lz => SAMPLE_LZ,
  }
  
  ROTATE_ANGLE = {
    0   => ANGLE000,
    90  => ANGLE090,
    180 => ANGLE180,
    270 => ANGLE270,
    360 => ANGLE000,
  }
  
  EXTENSIONS_FORMATS = {
    "jpeg" => JPEG,
    "jpg"  => JPEG,
    "png"  => PNG,
    "bmp"  => BITMAP,
  }
  
  EXTENSIONS = EXTENSIONS_FORMATS.keys
  FORMATS = EXTENSIONS_FORMATS.values.uniq
  
  PIX_LIMIT = 6000


##----soezimaster add.
  # dummy image ... umm
  DEFAULT_IMAGE = ''

  # COLORSPACE
  COLORSPACE = {
    :GRAY => 2,
    :RGB  => 3,
    :CMYK => 6
  }

  # COLORMODEL
  COLORMODEL = {
    :gray    => 0,
    :red     => 1,
    :green   => 2,
    :blue    => 3,
    :cyan    => 4,
    :magenta => 5,
    :yellow  => 6,
    :black   => 7
  }

  # dither-type
  DITHER = {
    :non => 0,
    :floydsteinberg => 1,
    :stucci => 2,
    :sierra => 3,
    :jajuni => 4
  }

  # focus-type
  FOCUS = {
    :detail => 0,
    :focus  => 1,
    :edges  => 2
  }

  # emboss-type
  EMBOSS = {
    :emboss    => 0,
    :heavy     => 1,
    :light     => 2,
    :laplacian => 3
  }

  # gray-type
  GRAY = {
    :nrm => 0,
    :med => 1,
    :ave => 2
  }

  # wave-type
  WAVE = {
    :horz => 0,
    :vert => 1,
    :both => 2
  }

  # pattern-type
  PATTERN = {
    :p_hexagon => 0,
    :p_diamond => 1,
    :p_circle  => 2,
    :p_brick   => 3
  }

  # paint-type
  PAINT = {
    :line => 0,
    :ave  => 1
  }

  # tile-type
  TILE = {
    :t_rect    => 0,
    :t_hexagon => 1,
    :t_diamond => 2,
    :t_circle  => 3,
    :t_brick   => 4
  }
  
  RESOLUTION = {
    :horz => 0,
    :vert => 1,
    :both => 2
  }
  
end
