module Pikl
  
  class Image
  
    # unshapmask
    def unshapmask(threshold, edge)
      error("invalid parameter. # => #{threshold}") if threshold.to_i<0 || threshold.to_i>255
      error("invalid parameter. # => #{edge}") if edge.to_f<-10.0 || edge.to_f>10.0
      Ext.pkl_unsharp(@pkl_image, threshold.to_i, edge.to_f)
      self
    end

    # コントラスト調整
    def contrast(rate)
      error("invalid parameter. # => #{rate}") if rate<-127 || rate>127
      Ext.pkl_contrast(@pkl_image, rate.to_i)
      self
    end

    # レベル補正
    def level(low, high, coeff)
      Ext.pkl_level(@pkl_image, low.to_f, high.to_f, coeff.to_f)
      self
    end

    # 明るさ調整
    def brightness(color)
      Ext.pkl_brightness(@pkl_image, color.to_i)
      self
    end
    
    # 輝度(明るさ)・彩度(鮮やかさ)・色相(色合い)調整
    def hls(ym, sm ,hd)
      error("invalid parameter. # => #{ym}") if ym.to_f<-1.0 || ym.to_f>1.0
      error("invalid parameter. # => #{sm}") if sm.to_f<-1.0 || sm.to_f>1.0
      Ext.pkl_hls(@pkl_image, ym.to_f, sm.to_f, hd.to_f)
      self
    end
    
    # gamma correction
    # _gm_ :: compensating rate.
    # gm must be >= 0 and image is neutral if gm value is 1.0.
    def gamma(gm)
      Ext.pkl_gamma(@pkl_image, gm.to_f)
      self
    end
    
    # compose image
    # _image_ :: another pikl image object.
    # _xpos_ :: left position of composing image.
    # _ypos_ :: top position of composing image.
    def compose(image, xpos, ypos)
      Ext.pkl_composite(@pkl_image, image.rawdata, xpos, ypos)
      self
    end
    
    # alphablend
    # _image_ :: another pikl image object.
    # _xpos_ :: left position of blend image.
    # _ypos_ :: top position of blend image.
    # _alpha_ :: alpha: α-value(0-255). 255 is complete transparent.
    def alphablend(image, xpos, ypos, alpha)
      Ext.pkl_alphablend(@pkl_image, image.rawdata, xpos, ypos, alpha);
      self
    end
    
    # mosaic effect
    # _width_ :: width for mosaic unit.
    # _height_ :: height for mosaic unit. 
    def mosaic(width, height)
      Ext.pkl_mosaic(@pkl_image, width, height)
      self
    end
    
    # 反転
    def invert
      Ext.pkl_invert(@pkl_image) if(@pkl_image)
    end
    
    # 手書き風
    def alphaedge(level=8, low=1.5, high=1.5, coeff=1.0, threshold=50, alpha=80)
      error("invalid parameter. # => #{level}") if level<1 || level>256
      error("invalid parameter. # => #{threshold}") if threshold<0 || threshold>80
      error("invalid parameter. # => #{alpha}") if alpha<0 || alpha>=256
      Ext.pkl_alphaedge(@pkl_image, level.to_i, low.to_f, high.to_f, coeff.to_f, threshold.to_i, alpha.to_i)
    end
    
    # クレヨン風
    def crayon(factor=15.0, bv1=3, bv2=2, alpha=80)
      error("invalid parameter. # => #{factor}") if factor.to_f<5.0 || factor.to_f>80.0
      error("invalid parameter. # => #{alpha}") if alpha<0 || alpha>=256
      Ext.pkl_crayon(@pkl_image, factor.to_f, bv1.to_i, bv2.to_i, alpha.to_i)
    end
  
    # ポスター風
    def edgeposter(level=4, low=1.5, high=1.5, coeff=1.0, edge=1)
      error("invalid parameter. # => #{level}") if level<2 || level>8
      error("invalid parameter. # => #{edge}") if edge<0 || edge>5
      Ext.pkl_edgeposter(@pkl_image, level.to_i, low.to_f, high.to_f, coeff.to_f, edge.to_i)
    end
  
    # あじさい風
    def hydrangea(zone=10, low=1.5, high=1.5, coeff=1.0, mil=50, env=70)
      error("invalid parameter. # => #{mil}") if mil.to_f<1 || mil.to_f>400
      error("invalid parameter. # => #{env}") if env.to_f<1 || env.to_f>100
      Ext.pkl_hydrangea(@pkl_image, zone.to_i, low.to_f, high.to_f, coeff.to_f, mil.to_f, env.to_f)
    end

    # 凝った輪郭
    def rinkaku(factor=30.0)
      Ext.pkl_rinkaku(@pkl_image, factor.to_f)
    end

    # voronoi-zone
    def voronoi_zone(zone, color=nil, edge=false)
      _color = nil
      unless color.nil?
        _color=color.rawdata if color.class.name == 'Pikl::Color'
      end
      _edge = edge ? 1 : 0
      Ext.pkl_voronoi_zone(@pkl_image, zone.to_i, _color, _edge)
    end

    # voronoi-count
    def voronoi_count(count, color=nil, edge=false)
      _color = nil
      unless color.nil?
        _color=color.rawdata if color.class.name == 'Pikl::Color'
      end
      _edge = edge ? 1 : 0
      Pikl::Ext.pkl_voronoi_count(@pkl_image, count.to_i, _color, _edge)
    end

    # sepia
    def sepia(red_weight, green_weight, blue_weight)
      error("invalid parameter. # => #{red_weight}") if red_weight.to_f<0.0 || red_weight.to_f>1.0
      error("invalid parameter. # => #{green_weight}") if green_weight.to_f<0.0 || green_weight.to_f>1.0
      error("invalid parameter. # => #{blue_weight}") if blue_weight.to_f<0.0 || blue_weight.to_f>1.0
      Ext.pkl_sepia(@pkl_image, red_weight.to_f, green_weight.to_f, blue_weight.to_f)
    end

    # oilpaint
    def oilpaint(weight=3)
      error("invalid parameter. # => #{weight}") if weight.to_i<2 || weight.to_i>6
      Ext.pkl_oilpaint(@pkl_image, weight.to_i)
    end

    # kuwahara
    def kuwahara(weight=3)
      error("invalid parameter. # => #{weight}") if weight.to_i<2 || weight.to_i>5
      Ext.pkl_kuwahara(@pkl_image, weight.to_i)
    end
    
    # blot
    def blot(level=3)
      Ext.pkl_noise(@pkl_image, level.to_i)
    end

    # vtr
    def vtr(colspace=3, gst=10, cst=10)
      Ext.pkl_vtr(@pkl_image, colspace.to_i, gst.to_i, cst.to_i)
    end

    # dither
    def dither(type=:floydsteinberg)
      error("invalid parameter. # => #{type}") unless DITHER.include?(type)
      Ext.pkl_dither(@pkl_image, DITHER[type])
    end

    # edgepaint
    def edgepaint(edge=1)
      Ext.pkl_edgepaint(@pkl_image, edge.to_i)
    end

    # edge
    #   threshold=0..30
    def edge(threshold=15)
      Ext.pkl_edge(@pkl_image, threshold.to_i)
    end

    def contour
      Ext.pkl_contour(@pkl_image)
    end

    def focus(type=:detail)
      error("invalid parameter. # => #{type}") unless FOCUS.include?(type)
      Ext.pkl_focus(@pkl_image, FOCUS[type])
    end

    def emboss(type=:light)
      error("invalid parameter. # => #{type}") unless EMBOSS.include?(type)
      Ext.pkl_emboss(@pkl_image, EMBOSS[type])
    end

    def emboss2(mask, w, h, factor=1.0, offset=0)
      error("invalid parameter. # => #{mask}") unless mask.class.name == 'Array'
      error("invalid parameter. # => #{mask} or #{w} or #{h}") unless mask.length == w.to_i*h.to_i
      Ext.pkl_emboss2(@pkl_image, mask, w.to_i, h.to_i, factor.to_f, offset.to_i)
    end

    def noisecut
      Ext.pkl_noisecut(@pkl_image)
    end

    def blur(weight)
      Ext.pkl_blur(@pkl_image, weight.to_i)
    end
    
    def gaussblur(weight)
      Ext.pkl_gaussblur(@pkl_image, weight.to_f)
    end
    
    def rblur(x=-1, y=-1, ef=30.0, weight=5)
      Ext.pkl_rblur(@pkl_image, x.to_i, y.to_i, ef.to_f, weight.to_i)
    end
    
    def ablur(x=-1, y=-1, ef=30.0, weight=5)
      Ext.pkl_ablur(@pkl_image, x.to_i, y.to_i, ef.to_f, weight.to_i)
    end
    
    def wblur(x=-1, y=-1, ef=30.0, weight=5, angle=45.0)
      Ext.pkl_wblur(@pkl_image, x.to_i, y.to_i, ef.to_f, weight.to_i, angle.to_f)
    end
    
    def mblur(angle, weight)
      Ext.pkl_mblur(@pkl_image, angle.to_f, weight.to_i)
    end

    def shadowframe(margin, backcolor=nil, shadowcolor=nil)
      _backcolor = nil
      unless backcolor.nil?
        _backcolor = backcolor.rawdata if backcolor.class.name == 'Pikl::Color'
      end
    
      _shadowcolor = nil
      unless shadowcolor.nil?
        _shadowcolor = shadowcolor.rawdata if shadowcolor.class.name == 'Pikl::Color'
      end
      
      Ext.pkl_shadowframe(@pkl_image, margin.to_i, _backcolor, _shadowcolor)
    end
  
  
    def randomshadow(child, x, y, margin, shadowcolor=nil)
      error("invalid parameter. # => #{child}") unless child.class.name == 'Pikl::Image'
      _shadowcolor = nil
      unless shadowcolor.nil?
        _shadowcolor = shadowcolor.rawdata if shadowcolor.class.name == 'Pikl::Color'
      end
      Pikl::Ext.pkl_randomshadow(@pkl_image, child.rawdata, x.to_i, y.to_i, margin.to_i, _shadowcolor)
    end

    def pixelate(ms)
      Ext.pkl_pixelate(@pkl_image, ms.to_i)
    end

    def grid(msx, msy, color)
      Ext.pkl_grid(@pkl_image, msx.to_i, msy.to_i, color.to_i)
    end
  
    def posterize(level)
      error("invalid parameter. # => #{level}") unless level>1 && level<256
      Ext.pkl_posterize(@pkl_image, level.to_i)
    end
  
    def cutcolor(level)
      Ext.pkl_cutcolor(@pkl_image, level)
    end
    
    # cmyk-model only
    def rgb
      Ext.pkl_rgb(@pkl_image)
    end
    
    def gray(type=:nrm)
      error("invalid parameter. # => #{type}") unless GRAY.include?(type)
      Ext.pkl_gray(@pkl_image, GRAY[type])
    end
  
    def blackwhite(level=127)
      Ext.pkl_2c(@pkl_image, level.to_i)
    end

    def tileslit(area, shift)
      Ext.pkl_tileslit(@pkl_image, area.to_i, shift.to_i)
    end

    def splitframe(backcolor, wbs, hbs, margin, frame)
      error("invalid parameter. # => #{backcolor}") unless backcolor != nil
      error("invalid parameter. # => #{backcolor}") unless backcolor.class.name == 'Pikl::Color'
      Ext.pkl_splitframe(@pkl_image, backcolor.rawdata, wbs.to_i, hbs.to_i, margin.to_i, frame.to_i)
    end
    
    def vignette(color, trans, radius, x=-1, y=-1)
      error("invalid parameter. # => #{color}") unless color != nil
      error("invalid parameter. # => #{color}") unless color.class.name == 'Pikl::Color'
      Ext.pkl_vignette(@pkl_image, color.rawdata, trans.to_i, radius.to_i, x.to_i, y.to_i)
    end

    def vaseline(radius, x=-1, y=-1)
      Ext.pkl_vaseline(@pkl_image, radius.to_i, x.to_i, y.to_i)
    end

    def vv(color, trans, radius, x=-1, y=-1)
      error("invalid parameter. # => #{color}") unless color != nil
      error("invalid parameter. # => #{color}") unless color.class.name == 'Pikl::Color'
      Ext.pkl_vv(@pkl_image, color.rawdata, trans.to_i, radius.to_i, x.to_i, y.to_i)
    end

    def film(color, trans)
      error("invalid parameter. # => #{color}") unless color != nil
      error("invalid parameter. # => #{color}") unless color.class.name == 'Pikl::Color'
      Ext.pkl_film(@pkl_image, color.rawdata, trans.to_i)
    end
  
    def swirl(factor, x, y, color)
      error("invalid parameter. # => #{color}") unless color != nil
      error("invalid parameter. # => #{color}") unless color.class.name == 'Pikl::Color'
      Ext.pkl_swirl(@pkl_image, factor.to_f, x.to_i, y.to_i, color.rawdata)
    end

    def wave(factor, frequency, type=:both)
      error("invalid parameter. # => #{type}") unless WAVE.include?(type)
      Ext.pkl_wave(@pkl_image, WAVE[type], factor.to_f, frequency.to_f)
    end
  
    def dots(zone, color)
      error("invalid parameter. # => #{color}") unless color != nil
      error("invalid parameter. # => #{color}") unless color.class.name == 'Pikl::Color'
      Ext.pkl_dots(@pkl_image, zone, color.rawdata)
    end

    def colordither(weight)
      Ext.pkl_colordither(@pkl_image, weight.to_i)
    end
  
    def fantasy(edgeweight, mix, saturation, hue)
      Ext.pkl_sobelpaint(@pkl_image, edgeweight.to_i, mix.to_f, saturation.to_f, hue.to_f)
    end
    
    def illust(gap, edge, gammaint)
      Ext.pkl_illust(@pkl_image, gap.to_i, edge.to_i, gammaint.to_i)
    end
  
    def color_emboss(mil, env)
      Ext.pkl_color_emboss(@pkl_image, mil.to_f, env.to_f)
    end

    def pattern(width, height, pattern=:p_hexagon, paint=:ave)
      error("invalid parameter. # => #{pattern}") unless PATTERN.include?(pattern)
      error("invalid parameter. # => #{paint}") unless PAINT.include?(paint)
      Ext.pkl_pattern(@pkl_image, width.to_i, height.to_i, PATTERN[pattern], PAINT[paint])
    end

    def tile(msx, msy, level, type=:t_rect)
      error("invalid parameter. # => #{type}") unless TILE.include?(type)
      Ext.pkl_tile(@pkl_image, TILE[type], msx.to_i, msy.to_i, level.to_i)
    end

    def affine(width, height, color, xscale, yscale, angle, x, y, sample=:bilinear)
      error("invalid parameter. # => #{type}") unless SAMPLES.include?(sample)
      error("invalid parameter. # => #{color}") unless color != nil
      error("invalid parameter. # => #{color}") unless color.class.name == 'Pikl::Color'
      Ext.pkl_affine(@pkl_image, SAMPLES[sample], width.to_i, height.to_i, color.rawdata, xscale.to_f, yscale.to_f, angle.to_f, x.to_i, y.to_i)
    end

  end
end



