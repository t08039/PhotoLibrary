require File.dirname(__FILE__) + '/test_helper.rb'

class TestPiklImage < Test::Unit::TestCase

  SAMPLE_IMAGE = File.dirname(__FILE__) + '/sample.jpg'
  SAMPLE_IMAGE2 = File.dirname(__FILE__) + '/sample2.jpg'
  SAMPLE_OUTPUT = File.dirname(__FILE__) + '/output.jpg'
  TESTOUT_DIR = File.dirname(__FILE__) + '/out'
  
  SAMPLE_IMAGE_W = 120
  SAMPLE_IMAGE_H = 160

  def setup
    #out‚Ì‰º‚ðÁ‚»‚¤I
  end

  def teardown
    if File.exists? SAMPLE_OUTPUT
      File.delete SAMPLE_OUTPUT
    end
  end


  def test_fdopen
    assert_raise(Pikl::ParameterException){
      f = "test"
      pk = Pikl::Image.new(Pikl::DEFAULT_IMAGE)
      pk.fdopen(f)
      pk.close
    }
    
    f = open(SAMPLE_IMAGE, "rb")
    pk = Pikl::Image.new(Pikl::DEFAULT_IMAGE)
    pk.fdopen(f)
    pk.save(TESTOUT_DIR + "/fdopen.jpg");
    pk.close
  end

  def test_dup
    assert_raise(Pikl::ParameterException){
      src = "test"
      pk = Pikl::Image.new(Pikl::DEFAULT_IMAGE)
      pk.dup(src)
      pk.close
    }
    
    src = Pikl::Image.new(SAMPLE_IMAGE)
    pk = Pikl::Image.new(Pikl::DEFAULT_IMAGE)
    pk.dup(src)
    src.close
    pk.save(TESTOUT_DIR + "/dup.jpg");
    pk.close
  end

  def test_canvas
    pk = Pikl::Image.new(Pikl::DEFAULT_IMAGE)
    pk.canvas(100, 100, :GRAY, Pikl::Color.gray(100))
    pk.save(TESTOUT_DIR + "/canvas_gray.jpg")
    pk.close
    
    pk = Pikl::Image.new(Pikl::DEFAULT_IMAGE)
    pk.canvas(100, 100, :RGB, Pikl::Color.rgb(0xff, 0xff, 0xff))
    pk.save(TESTOUT_DIR + "/canvas_rgb.jpg")
    pk.close
    
    pk = Pikl::Image.new(Pikl::DEFAULT_IMAGE)
    pk.canvas(100, 100, :CMYK, Pikl::Color.cmyk(0xff, 0, 0xff, 0xff))
    pk.save(TESTOUT_DIR + "/canvas_cmyk.jpg")
    pk.close
  end

  def test_output
    f = open(TESTOUT_DIR+'/output.jpg', "wb")
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.output(f, pk.format)
    #pk.output($stdout, pk.format)
    pk.close
    f.close
  end
  
  def test_dpi
    pk = Pikl::Image.new(SAMPLE_IMAGE2)
    assert_equal(pk.get_dpi(:horz), 100.0)
    assert_equal(pk.get_dpi(:vert), 100.0)
    pk.close
    
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.set_dpi(300.0)
    pk.save(TESTOUT_DIR + "/res.jpg")
    pk.close
    
    pk = Pikl::Image.new(SAMPLE_IMAGE2)
    pk.save(TESTOUT_DIR + "/res2.jpg")
    pk.close
  end
  
  def test_count
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    assert_equal(pk.count, 12979)
    pk.close
  end
  
  def test_colorspace
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    assert_equal(pk.colorspace, :RGB)
    pk.close
  end
  
  def test_getcolor
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    assert_equal(pk.getcolor(0, 0, :red), 207)
    assert_equal(pk.getcolor(0, 0, :green), 206)
    assert_equal(pk.getcolor(0, 0, :blue), 201)
    pk.close
  end
  
  def test_setcolor
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    for y in 0..100 do
      for x in 0..100 do
        pk.setcolor(x, y, :red, 255)
      end
    end
    pk.save(TESTOUT_DIR + "/setcolor.jpg")
    pk.close
  end
  
  def test_getpixel
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    color = pk.getpixel(0,0)
    assert_equal(color['red'], 207)
    assert_equal(color['green'], 206)
    assert_equal(color['blue'], 201)
    pk.close
  end
  
  def test_setpixel
    color = Pikl::Color.rgb(100,100,100)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    for y in 0..100 do
      for x in 0..100 do
        pk.setpixel(x, y, color, false)
      end
    end
    pk.save(TESTOUT_DIR + "/setpixel.jpg")
    pk.close
    color.close
  end
  
  def test_invert
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.invert
    pk.save(TESTOUT_DIR + "/invert.jpg")
    pk.close
  end
  
  def test_alphaedge
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.alphaedge(8, 1.5, 1.5, 1.0, 30, 50)
    pk.save(TESTOUT_DIR + "/alphaedge.jpg")
    pk.close
  end
  
  def test_crayon
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.crayon
    pk.save(TESTOUT_DIR + "/crayon.jpg")
    pk.close
  end
  
  def test_edgeposter
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.edgeposter(4, 9.0, 9.0, 1.0, 1)
    pk.save(TESTOUT_DIR + "/edgeposter.jpg")
    pk.close
  end
  
  def test_hydrangea
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.hydrangea
    pk.save(TESTOUT_DIR + "/hydrangea.jpg")
    pk.close
  end
  
  def test_rinkaku
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.rinkaku
    pk.save(TESTOUT_DIR + "/rinkaku.jpg")
    pk.close
  end
  
  def test_voronoi_zone
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.voronoi_zone(5)
    pk.save(TESTOUT_DIR + "/voronoi_zone.jpg")
    pk.close
    
    color = Pikl::Color.rgb(0, 0, 0)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.voronoi_zone(5, color)
    pk.save(TESTOUT_DIR + "/voronoi_zone_edge.jpg")
    pk.close
    color.close
    
  end
  
  def test_voronoi_count
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.voronoi_count(300)
    pk.save(TESTOUT_DIR + "/voronoi_count.jpg")
    pk.close
    
    color = Pikl::Color.rgb(0, 0, 0)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.voronoi_count(300, color)
    pk.save(TESTOUT_DIR + "/voronoi_count_edge.jpg")
    pk.close
    color.close
  end
  
  def test_sepia
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.sepia(0.89, 0.8, 0.5)
    pk.save(TESTOUT_DIR + "/sepia.jpg")
    pk.close
  end
  
  def test_oilpaint
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.oilpaint(4)
    pk.save(TESTOUT_DIR + "/oilpaint.jpg")
    pk.close
  end
  
  def test_kuwahara
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.kuwahara(4)
    pk.save(TESTOUT_DIR + "/kuwahara.jpg")
    pk.close
  end
  
  def test_blot
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.blot(4)
    pk.save(TESTOUT_DIR + "/blot.jpg")
    pk.close
  end
  
  def test_vtr
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.vtr(3, 10, 10)
    pk.save(TESTOUT_DIR + "/vtr.jpg")
    pk.close
  end
  
  def test_dither
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.dither
    pk.save(TESTOUT_DIR + "/dither.jpg")
    pk.close
  end
  
  def test_edgepaint
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.edgepaint
    pk.save(TESTOUT_DIR + "/edgepaint.jpg")
    pk.close
  end

  def test_edge
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.edge
    pk.save(TESTOUT_DIR + "/edge.jpg")
    pk.close
  end
  
  def test_contour
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.contour
    pk.save(TESTOUT_DIR + "/contour.jpg")
    pk.close
  end

  def test_focus
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.focus
    pk.save(TESTOUT_DIR + "/focus.jpg")
    pk.close
  end
  
  def test_emboss
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.emboss
    pk.save(TESTOUT_DIR + "/emboss.jpg")
    pk.close
  end
  
  def test_custom_emboss
    mask = [-1, -1, -1,
            -1,  8, -1,
            -1, -1, -1]
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.emboss2(mask, 3, 3)
    pk.save(TESTOUT_DIR + "/custom_emboss.jpg")
    pk.close
  end
  
  def test_noisecut
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.noisecut
    pk.save(TESTOUT_DIR + "/noisecut.jpg")
    pk.close
  end
  
  def test_blur
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.blur(3)
    pk.save(TESTOUT_DIR + "/blur.jpg")
    pk.close
    
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.gaussblur(3.0)
    pk.save(TESTOUT_DIR + "/gaussblur.jpg")
    pk.close
    
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.rblur
    pk.save(TESTOUT_DIR + "/rblur.jpg")
    pk.close
    
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.ablur
    pk.save(TESTOUT_DIR + "/ablur.jpg")
    pk.close
    
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.wblur
    pk.save(TESTOUT_DIR + "/wblur.jpg")
    pk.close
    
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.mblur(30.0, 5)
    pk.save(TESTOUT_DIR + "/mblur.jpg")
    pk.close
  end
  
  def test_shadowframe
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.shadowframe(10)
    pk.save(TESTOUT_DIR + "/shadowframe.jpg")
    pk.close
  end
  
  def test_randomshadow
    pk = Pikl::Image.new(SAMPLE_IMAGE)
  
    canvas = Pikl::Image.new(Pikl::DEFAULT_IMAGE)
    canvas.canvas(300, 300, :RGB, Pikl::Color.rgb(0xff, 0xff, 0xff))

    canvas.randomshadow(pk, 100, 100, 10)
    canvas.randomshadow(pk, 50, 50, 10)
    canvas.randomshadow(pk, 0, 0, 10)

    canvas.save(TESTOUT_DIR + "/randomshadow.jpg")
    canvas.close
    pk.close
  end
  
  
  def test_color_emboss
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.color_emboss(100, 70)
    pk.save(TESTOUT_DIR + "/color_emboss.jpg")
    pk.close
  end
  
  def test_grid
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.grid(5, 5, 100)
    pk.save(TESTOUT_DIR + "/grid.jpg")
    pk.close
  end

  def test_posterize
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.posterize(8)
    pk.save(TESTOUT_DIR + "/posterize.jpg")
    pk.close
  end
  
  def test_cutcolor
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.cutcolor(0b10101100)
    pk.save(TESTOUT_DIR + "/cutcolor.jpg")
    pk.close
  end
  
  def test_gray
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.gray
    pk.save(TESTOUT_DIR + "/gray.jpg")
    pk.close
    
    pk = Pikl::Image.new(TESTOUT_DIR + "/gray.jpg")
    pk.blackwhite
    pk.save(TESTOUT_DIR + "/blackwhite.jpg")
    pk.close
  end
  
  def test_tileslit
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.tileslit(10,5)
    pk.save(TESTOUT_DIR + "/tileslit.jpg")
    pk.close
  end
  
  def test_splitframe
    color = Pikl::Color.rgb(200,100,255)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.splitframe(color, 5, 5, 3, 2)
    pk.save(TESTOUT_DIR + "/splitframe.jpg")
    pk.close
  end
  
  def test_vignette
    color = Pikl::Color.rgb(0,0,0)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.vignette(color, 50, 80)
    pk.save(TESTOUT_DIR + "/vignette.jpg")
    pk.close
    color.close
  end
  
  def test_vaseline
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.vaseline(80)
    pk.save(TESTOUT_DIR + "/vaseline.jpg")
    pk.close
  end
  
  def test_vv
    color = Pikl::Color.rgb(0,0,0)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.vv(color, 50, 80)
    pk.save(TESTOUT_DIR + "/vv.jpg")
    pk.close
    color.close
  end
  
  def test_film
    color = Pikl::Color.rgb(200,100,150)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.film(color, 30)
    pk.save(TESTOUT_DIR + "/film.jpg")
    pk.close
    color.close
  end
  
  def test_swirl
    color = Pikl::Color.rgb(200,100,150)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.swirl(0.005, -1, -1, color)
    pk.save(TESTOUT_DIR + "/swirl.jpg")
    pk.close
    color.close
  end
  
  def test_wave
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.wave(15.0, 5.0)
    pk.save(TESTOUT_DIR + "/wave.jpg")
    pk.close
  end
  
  def test_dots
    color = Pikl::Color.rgb(200,100,150)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.dots(3, color)
    pk.save(TESTOUT_DIR + "/dots.jpg")
    pk.close
    color.close
  end
  
  def test_colordither
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.colordither(50)
    pk.save(TESTOUT_DIR + "/colordither.jpg")
    pk.close
  end
  
  def test_fantasy
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.fantasy(1, 0.5, 1.0, 0)
    pk.save(TESTOUT_DIR + "/fantasy.jpg")
    pk.close
  end
  
  def test_illust
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.illust(50,20,50)
    pk.save(TESTOUT_DIR + "/illust.jpg")
    pk.close
  end
  
  def test_pattern
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.pattern(3,9)
    pk.save(TESTOUT_DIR + "/pattern.jpg")
    pk.close
  end
  
  def test_tile
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.tile(5,5,50)
    pk.save(TESTOUT_DIR + "/tile.jpg")
    pk.close
  end
  
  def test_affine
    color = Pikl::Color.rgb(200,100,150)
    pk = Pikl::Image.new(SAMPLE_IMAGE)
    pk.affine(300, 300, color, 2.0, 2.0, 30.0, 50, 50)
    pk.save(TESTOUT_DIR + "/affine.jpg")
    pk.close
    color.close
  end
  
  
  def test_special
    Pikl::Image.open(SAMPLE_IMAGE) do |img|
      img.posterize(8)
      img.noisecut
      
      img2 = Pikl::Image.new(Pikl::DEFAULT_IMAGE)
      img2.dup(img)
      img2.level(5.0, 5.0, 1.0)
      
      img.brightness(50)
      img.edge(30)
      
      img.alphablend(img2, 0, 0, 70)
      
      img.save(TESTOUT_DIR + "/special.jpg")
      img.close
      img2.close
    end
  end
  
end


