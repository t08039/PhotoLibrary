module Pikl
  
  class Color
    
    def initialize(a,b,c,d)
      validate_color(a)
      validate_color(b)
      validate_color(c)
      validate_color(d)
      @color = Pikl::Ext.pkl_color_create(a,b,c,d)
    end

    def Color.gray(g)
      new(g,0,0,0)
    end

    def Color.rgb(r, g, b)
      new(r,g,b,0)
    end

    def Color.cmyk(c,m,y,k)
      new(c,m,y,k)
    end
    
    #attr_accessor :color
    def rawdata
      @color
    end
    
    def getcolor(colormodel)
      error("invalid colormodel parameter. # => #{colormodel}") unless COLORMODEL.include?(colormodel)
      Ext.pkl_color(@color, COLORMODEL[colormodel])
    end

    def close
      Ext.pkl_color_close(@color)
      @color=nil
    end
    
    def validate_color(v)
      error("invalid color parameter. # => #{v}") unless /^\d+$/ =~ v.to_s
      error("invalid color parameter. # => #{v}") unless v.to_i >= 0 && v.to_i <= 255
    end
    
  end
end

