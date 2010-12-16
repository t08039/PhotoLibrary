require File.dirname(__FILE__) + '/test_helper.rb'

class TestPiklImage < Test::Unit::TestCase

  SAMPLE_IMAGE = File.dirname(__FILE__) + '/sample.jpg'
  SAMPLE_OUTPUT = File.dirname(__FILE__) + '/output.jpg'
  
  SAMPLE_IMAGE_W = 120
  SAMPLE_IMAGE_H = 160

  def setup
    
  end

  def teardown
    if File.exists? SAMPLE_OUTPUT
      File.delete SAMPLE_OUTPUT
    end
  end
  
  def test_width_height
    # + test open method without block.
    img = Pikl::Image.open(SAMPLE_IMAGE)
    assert_equal(SAMPLE_IMAGE_W, img.width)
    assert_equal(SAMPLE_IMAGE_H, img.height)
    img.close()
    
  end
  
  def test_fit_inner
    #sample.jpg must be 120x160.
    img = Pikl::Image.open(SAMPLE_IMAGE)
    img.fit(60,81)
    assert_equal(60, img.width)
    assert_equal(80, img.height)
    img.close()

    img = Pikl::Image.open(SAMPLE_IMAGE)
    img.fit(61,80)
    assert_equal(60, img.width)
    assert_equal(80, img.height)
    img.close()
    
    img = Pikl::Image.open(SAMPLE_IMAGE)
    img.fit(240,321)
    assert_equal(240, img.width)
    assert_equal(320, img.height)
    img.close()

    img = Pikl::Image.open(SAMPLE_IMAGE)
    img.fit(361,480)
    assert_equal(360, img.width)
    assert_equal(480, img.height)
    img.close()
  end

  def test_fit_outer
    #sample.jpg must be 120x160.
    img = Pikl::Image.open(SAMPLE_IMAGE)
    img.fit(360,479,:outer)
    assert_equal(360, img.width)
    assert_equal(480, img.height)
    img.close()

    img = Pikl::Image.open(SAMPLE_IMAGE)
    img.fit(359,480,:outer)
    assert_equal(360, img.width)
    assert_equal(480, img.height)
    img.close()
    
    img = Pikl::Image.open(SAMPLE_IMAGE)
    img.fit(119,160,:outer)
    assert_equal(120, img.width)
    assert_equal(160, img.height)
    img.close()

    img = Pikl::Image.open(SAMPLE_IMAGE)
    img.fit(120,159,:outer)
    assert_equal(120, img.width)
    assert_equal(160, img.height)
    img.close()
  end
  
  def test_trim  
    Pikl::Image.open(SAMPLE_IMAGE) do |img|

      img.trim(0,0,:auto,:auto)
      assert_equal(SAMPLE_IMAGE_W, img.width)
      assert_equal(SAMPLE_IMAGE_H, img.height)
      img.save(SAMPLE_OUTPUT)
      
      Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
        assert_equal(SAMPLE_IMAGE_W,imgout.width)
        assert_equal(SAMPLE_IMAGE_H,imgout.height)
      end

      img.trim(5,10,:auto,:auto)
      assert_equal(SAMPLE_IMAGE_W-5, img.width)
      assert_equal(SAMPLE_IMAGE_H-10, img.height)
      img.save(SAMPLE_OUTPUT)

      Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
        assert_equal(SAMPLE_IMAGE_W-5, imgout.width)
        assert_equal(SAMPLE_IMAGE_H-10, imgout.height)
      end

      img.trim(0,0,-1,-3)
      assert_equal(SAMPLE_IMAGE_W-5-1, img.width)
      assert_equal(SAMPLE_IMAGE_H-10-3, img.height)
      img.save(SAMPLE_OUTPUT)

      Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
        assert_equal(SAMPLE_IMAGE_W-5-1, imgout.width)
        assert_equal(SAMPLE_IMAGE_H-10-3, imgout.height)
      end

      img.trim(2,3,4,5)
      assert_equal(4, img.width)
      assert_equal(5, img.height)
      img.save(SAMPLE_OUTPUT)

      Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
        assert_equal(4, imgout.width)
        assert_equal(5, imgout.height)
      end

      #bad args
      assert_raise(Pikl::ParameterException){img.trim(0,0,0,0)}
    
    end
    
  end
    
    def test_rotate
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
    
        img.rotate(90)
        assert_equal(SAMPLE_IMAGE_H, img.width)
        assert_equal(SAMPLE_IMAGE_W, img.height)
        img.save(SAMPLE_OUTPUT)
        
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_H,imgout.width)
          assert_equal(SAMPLE_IMAGE_W,imgout.height)
        end
        
        img.rotate(90)
        assert_equal(SAMPLE_IMAGE_W, img.width)
        assert_equal(SAMPLE_IMAGE_H, img.height)
        img.save(SAMPLE_OUTPUT)
        
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_W,imgout.width)
          assert_equal(SAMPLE_IMAGE_H,imgout.height)
        end
    
        img.rotate(180)
        assert_equal(SAMPLE_IMAGE_W, img.width)
        assert_equal(SAMPLE_IMAGE_H, img.height)
        img.save(SAMPLE_OUTPUT)
        
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_W,imgout.width)
          assert_equal(SAMPLE_IMAGE_H,imgout.height)
        end
    
        img.rotate(270)
        assert_equal(SAMPLE_IMAGE_H, img.width)
        assert_equal(SAMPLE_IMAGE_W, img.height)
        img.save(SAMPLE_OUTPUT)
        
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_H,imgout.width)
          assert_equal(SAMPLE_IMAGE_W,imgout.height)
        end

        img.rotate(285,"#000000")
        img.save(SAMPLE_OUTPUT)
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(157,imgout.width)
          assert_equal(185,imgout.height)
        end
      end
    end
    
    def test_resize
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        img.resize(50,:auto)
        img.save(SAMPLE_OUTPUT)
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(50,imgout.width)
          assert_equal((SAMPLE_IMAGE_H*50/SAMPLE_IMAGE_W).to_i,imgout.height)
        end
      end
    end

    def test_fit_inner2
      # vertical-inner
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        img.fit(20,100)
        img.save(SAMPLE_OUTPUT)
        assert_equal(20,img.width)
        assert_equal((SAMPLE_IMAGE_H*20/SAMPLE_IMAGE_W).to_i,img.height)
      end
      
      # horizontal-inner
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        img.fit(100,20)
        img.save(SAMPLE_OUTPUT)
        assert_equal(20,img.height)
        assert_equal((SAMPLE_IMAGE_W*20/SAMPLE_IMAGE_H).to_i,img.width)
      end

      # vertical-inner
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        img.fit(200,500)
        assert_equal(200,img.width)
        assert_equal((SAMPLE_IMAGE_H*200/SAMPLE_IMAGE_W).to_i,img.height)

      end
      
      # horizontal-inner
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        img.fit(500,200)
        assert_equal(200,img.height)
        assert_equal((SAMPLE_IMAGE_W*200/SAMPLE_IMAGE_H).to_i,img.width)
      end

    end
    
    def test_vh
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        assert_equal(true,img.is_vertical?)
        assert_equal(false,img.is_horizontal?)
        img.rotate(90)
        assert_equal(false,img.is_vertical?)
        assert_equal(true,img.is_horizontal?)
      end
    end

    def test_effect
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        img.unshapmask(255, 10)
        img.save(SAMPLE_OUTPUT)
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_W,imgout.width)
          assert_equal(SAMPLE_IMAGE_H,imgout.height)
        end
    
        img.contrast(127)
        img.save(SAMPLE_OUTPUT)
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_W,imgout.width)
          assert_equal(SAMPLE_IMAGE_H,imgout.height)
        end
        
        img.level(1.0, 0.1, 0.95)
        img.save(SAMPLE_OUTPUT)
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_W,imgout.width)
          assert_equal(SAMPLE_IMAGE_H,imgout.height)
        end
        
        img.brightness(1)
        img.save(SAMPLE_OUTPUT)
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_W,imgout.width)
          assert_equal(SAMPLE_IMAGE_H,imgout.height)
        end
        
        img.hls(1,1,113.2)
        img.save(SAMPLE_OUTPUT)
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_W,imgout.width)
          assert_equal(SAMPLE_IMAGE_H,imgout.height)
        end
    
        img.gamma(1.5)
        img.save(SAMPLE_OUTPUT)
        Pikl::Image.open(SAMPLE_OUTPUT) do |imgout|
          assert_equal(SAMPLE_IMAGE_W,imgout.width)
          assert_equal(SAMPLE_IMAGE_H,imgout.height)
        end
      end
    end
    
    def test_validate_trim
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        assert_nil(img.validate_trim(0,0,1,1))
    
        assert_raise(Pikl::ParameterException){img.validate_trim(0,0,0,0)}
        assert_raise(Pikl::ParameterException){img.validate_trim(0,0,0,1)}
        assert_raise(Pikl::ParameterException){img.validate_trim(0,0,1,0)}
        
        assert_raise(Pikl::ParameterException){img.validate_trim("A",0,1,1)}
        assert_raise(Pikl::ParameterException){img.validate_trim(0,"B",1,1)}
        assert_raise(Pikl::ParameterException){img.validate_trim(0,0,"C",1)}
        assert_raise(Pikl::ParameterException){img.validate_trim(0,0,1,"D")}
    
        assert_nil(img.validate_trim(0,0,1,:auto))
        assert_nil(img.validate_trim(0,0,:auto,1))
        assert_nil(img.validate_trim(0,0,:auto,:auto))  
    
        assert_nil(img.validate_trim(0,0,1,Pikl::PIX_LIMIT))
        assert_nil(img.validate_trim(0,0,Pikl::PIX_LIMIT,1))
        assert_nil(img.validate_trim(0,0,Pikl::PIX_LIMIT,Pikl::PIX_LIMIT))
        assert_raise(Pikl::ParameterException){img.validate_trim(0,0,1,Pikl::PIX_LIMIT+1)}
        assert_raise(Pikl::ParameterException){img.validate_trim(0,0,Pikl::PIX_LIMIT+1,1)}
    
        assert_nil(img.validate_trim(0,0,1,-1))
        assert_nil(img.validate_trim(0,0,-1,1))
      end
      
    end
    
    def test_validate_resize
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
    
        assert_raise(Pikl::ParameterException){ img.validate_resize("hoge",900) }
        assert_raise(Pikl::ParameterException){ img.validate_resize(600,"fuga") }
    
        assert_raise(Pikl::ParameterException){img.validate_resize(30,Pikl::PIX_LIMIT+1)}
        assert_raise(Pikl::ParameterException){img.validate_resize(Pikl::PIX_LIMIT+1,10)}
        assert_nil(img.validate_resize(30,Pikl::PIX_LIMIT))
        assert_nil(img.validate_resize(Pikl::PIX_LIMIT,10))
    
        assert_raise(Pikl::ParameterException){img.validate_resize(30,0)}
        assert_raise(Pikl::ParameterException){img.validate_resize(0,10)}
        assert_nil(img.validate_resize(1,10))
        assert_nil(img.validate_resize(10,1))
      
        assert_raise(Pikl::ParameterException){img.validate_resize(:auto,:auto)}
        assert_nil(img.validate_resize(600,:auto))
        assert_nil(img.validate_resize(:auto,6))
    
      end
    end

    def test_format
      Pikl::Image.open(SAMPLE_IMAGE) do |img|    
        assert_equal(Pikl::JPEG, img.format)
      end
    end
    
    def test_compress
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        img.trim(5,10,:auto,:auto)
        img.save(SAMPLE_OUTPUT,Pikl::JPEG,5)
      end
    end
    
    # mosaic effect
    # _width_ :: width for mosaic unit.
    # _height_ :: height for mosaic unit. 
    def test_mosaic
      Pikl::Image.open(SAMPLE_IMAGE) do |img|
        img.mosaic(3,3)
      end
    end
    
    # compose image
    # _image_ :: another pikl image object.
    # _xpos_ :: left position of composing image.
    # _ypos_ :: top position of composing image.
    # def test_compose
    #   Pikl::Image.open(SAMPLE_IMAGE) do |img2|
    #     Pikl::Image.open(SAMPLE_IMAGE) do |img|
    #       img.compose(img2,3,3)
    #     end
    #   end
    # end

    # # mediancut to decrease colors.
    # # _ncolors_ :: num of colors.(1..256)
    # # _dither_ :: dither. true/false
    # def test_mediancut
    #   Pikl::Image.open(SAMPLE_IMAGE) do |img|
    #     img.mediancut(256,true)
    #   end
    # end

    # alphablend
    # _image_ :: another pikl image object.
    # _xpos_ :: left position of blend image.
    # _ypos_ :: top position of blend image.
    # _alpha_ :: alpha: α-value(0-255). 255 is complete transparent.
    def test_alphablend
      Pikl::Image.open(SAMPLE_IMAGE) do |img2|
        Pikl::Image.open(SAMPLE_IMAGE) do |img|
          img.alphablend(img2,0,0,200)
        end
      end
    end
    
    # def test_method_chain
    #   Pikl::Image.open(SAMPLE_IMAGE) do |img2|
    #     Pikl::Image.open(SAMPLE_IMAGE) do |img|
    #       img.trim(5,10,:auto,:auto).rotate(90).resize(50,:auto).unshapmask(10,3).contrast(10).level(5,5,1.2).brightness(10).hls(0.1,0.1,355).gamma(1.2).mosaic(3,3).mediancut(256,true).compose(img2,10,10).save(SAMPLE_OUTPUT)
    #     end
    #   end
    # end
    

end
