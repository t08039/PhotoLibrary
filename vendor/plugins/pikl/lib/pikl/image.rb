module Pikl
  
  class Image

    def self.open(inpath, &block)
      image = Pikl::Image.new(inpath)
      image.instance_variable_set(:@block,block)
      
      return image unless block_given?

      begin
        block.call(image)
      ensure
        image.close if image
      end
    
    end
    
    def rawdata
      @pkl_image
    end
    
    def initialize( inpath )
      @pkl_image = Ext.pkl_open(File.expand_path(inpath))
    end

    def save(outpath, format = nil, compress = nil)
      raise Pikl::ImageProcessException.new("image already closed.") unless @pkl_image
      format ||= (split_extensions(outpath) || @format)
      validate_format(format)
      if compress
        validate_compress(compress)
        Ext.pkl_compress(@pkl_image, compress.to_i)
      end
      Ext.pkl_save(@pkl_image, File.expand_path(outpath), EXTENSIONS_FORMATS[format.to_s.downcase] || format.to_i)
      self.close unless(@block)
      @pkl_image
    end
    
    def close()
      Ext.pkl_close(@pkl_image) if @pkl_image
      @pkl_image = nil
    end
    
    def width
      Ext.pkl_width(@pkl_image) if(@pkl_image)
    end
    
    def height
      Ext.pkl_height(@pkl_image) if(@pkl_image)
    end
    
    def format
      Ext.pkl_format(@pkl_image) if(@pkl_image)
    end
    
    def trim(x, y, dist_x, dist_y)
      validate_trim(x, y, dist_x, dist_y)
      dist_x = trim_dist(x, self.width, dist_x)
      dist_y = trim_dist(y, self.height, dist_y)
            
      Ext.pkl_trim(@pkl_image, x, y, dist_x, dist_y)
      
      self
    end
    
    def trim_dist(start, dist_from, dist_to)
      if dist_to == :auto || dist_to > (dist_from - start)
        dist_to = dist_from - start
      elsif dist_to < 0
        dist_to += (dist_from - start)
      else
        dist_to
      end
    end

    def rotate(angle, backcolor = "#FFFFFF", sample = :bilinear)
      ci = color_int(backcolor)
      color = Ext.pkl_color_rgb(ci[0], ci[1], ci[2])
      Ext.pkl_rotate(@pkl_image, angle.to_f, SAMPLES[sample.to_sym], color)
      Ext.pkl_color_close(color)
      self
    end
    
    def fit(width, height, mode = :inner)
      validate_fit_mode(mode)
      self.send("fit_#{mode.to_s}", width, height)
    end
    
    def fit_inner(width, height)
      to_w, to_h = ((width.to_f / self.width.to_f) < (height.to_f / self.height.to_f)) ? [width, :auto] : [:auto, height]
      resize(to_w,to_h)
    end
    
    def fit_outer(width, height)
      to_w, to_h = ((width.to_f / self.width.to_f) > (height.to_f / self.height.to_f)) ? [width, :auto] : [:auto, height]
      resize(to_w,to_h)
    end
    
    def color_int(colorstr)
      validate_color(colorstr)
      cs = colorstr.gsub(/^#/,"")
      color = []
      0.step(6,2) do |i|
        color << ( cs[i,2].to_s.empty? ? "FF" : cs[i,2] )
      end
      color.map!{|c| c.hex}
    end

    def validate_color(v)
      error("invalid color parameter. # => #{v}") unless  /^#[a-f|A-F|0-9]+$/ =~ v.to_s
    end

    def resize(width, height, sample = :pixcel_averate)
      validate_auto(width,height)

      case sample.class.name
      when 'Symbol', 'String'
        sample = SAMPLES[sample.to_sym]
      end
      
      width = self.width * height / self.height if(width == :auto)
      height = self.height * width / self.width if(height == :auto)
      
      validate_resize(width, height)

      Ext.pkl_resize(@pkl_image, width, height, sample)
      self
    end

    def is_vertical?
      self.height > self.width
    end

    def is_horizontal?
      self.height <= self.width
    end

    # regular expressions to try for identifying extensions
    def split_extensions(path)
      filename = path.split('/').last
      (filename =~ %r|.+\.([a-z,A-Z]+)$|) ? $1 : nil
    end

    def validate_auto(width, height)
      error("invalid :autoto in the both of width and height.") if(width == :auto && height == :auto)
    end

    # validate identifying extensions
    def validate_format(ext)
      error(ext) unless (ext || EXTENSIONS.include?(ext.to_s.downcase) || FORMATS.include?(ext.to_i))
    end

    def validate_trim(x, y, dist_x, dist_y)
      validate_numeric(x, y)
      
      validate_pix(dist_x, true)
      validate_pix(dist_y, true)

      error("left is outside the range.  #{x}") if x > self.width.to_i
      error("top is outside the range.  #{y}") if y > self.height.to_i
    end
    
    def validate_resize(width, height)
      validate_auto(width, height)
      validate_pix(width)
      validate_pix(height)
    end

    def validate_pix(value, allow_minus = false)
      return if value == :auto
      validate_numeric(value)
      error("value is outside the range.  #{value}") if value == 0
      error("value is outside the range.  #{value}") if !allow_minus && value < 0
      error("value is outside the range.  #{value}") if value > PIX_LIMIT
    end

    def validate_numeric(*args)
      args = [args] unless args.is_a?(Array)
      args.each do |v|
        error("invalid parameter. # => #{v}") unless /^[-\d]+$/ =~ v.to_s
      end      
    end
    
    def validate_compress(v)
      error("invalid compress parameter. # => #{v}") unless /^\d+$/ =~ v.to_s
      error("invalid compress parameter. # => #{v}") unless v.to_i >= 0 && v.to_i <= 10
    end
    
    def validate_fit_mode(v)
        error("invalid fit parameter. # => #{v}") unless [:inner, :outer].include?(v)
    end

    def error(message)
      #self.close
      raise Pikl::ParameterException.new(message)
    end
    

##--soezimaster add.
    # オープン済みファイルディスクリプタからPiklを生成する
    def fdopen(fd)
      self.close
      error("invalid parameter. # => #{fd}") unless(fd.class.name=='File' || fd.class.name=='Tempfile')
      @pkl_image = Ext.pkl_fdopen(fd)
    end
    
    # 複製
    def dup(src)
      self.close
      error("invalid parameter. # => #{src}") unless src.class.name == 'Pikl::Image'
      @pkl_image = Ext.pkl_dup(src.rawdata)
    end
    
    # 新規のキャンバスを作成する
    def canvas(width, height, colorspace, color)
      self.close
      error("invalid parameter. # => #{color}") if color.nil?
      error("invalid parameter. # => #{color}") if color.class.name != 'Pikl::Color'
      error("invalid parameter. # => #{colorspace}") unless COLORSPACE.include?(colorspace)
      validate_pix(width)
      validate_pix(height)
      @pkl_image = Ext.pkl_canvas(width, height, COLORSPACE[colorspace], color.rawdata)
    end
    
    # オープン済みファイルディスクリプタ(もしくはIO)にイメージを書き出す
    def output(fd, format, compress=nil)
      raise Pikl::ImageProcessException.new("image already closed.") unless @pkl_image
      error("invalid parameter. # => #{fd}") unless ['File', 'IO'].include?(fd.class.name)
      validate_format(format)
      if compress
        validate_compress(compress)
        Ext.pkl_compress(@pkl_image, compress.to_i)
      end
      
      Ext.pkl_output(@pkl_image, fd, EXTENSIONS_FORMATS[format.to_s.downcase] || format.to_i)
      self.close unless(@block)
      @pkl_image
    end
    
    # 色数を数える
    def count
      Ext.pkl_count(@pkl_image) if(@pkl_image)
    end
    
    # カラースペースを返す
    def colorspace
      nil unless(@pkl_image)
      COLORSPACE.find{ |k, v|
        v == Ext.pkl_colorspace(@pkl_image)
        return k
      }
    end

    # 解像度を設定する
    def set_dpi(res, type=:both)
      Ext.pkl_set_dpi(@pkl_image, Pikl::RESOLUTION[type], res) if(@pkl_image)
    end

    # 解像度を取得する
    def get_dpi(type)
      Ext.pkl_get_dpi(@pkl_image, Pikl::RESOLUTION[type]) if(@pkl_image)
    end
    
    # 特定ピクセルの色を取り出す
    def getcolor(x, y, colormodel)
      nil unless(@pkl_image)
      validate_position(x, y)
      error("invalid parameter. # => #{colormodel}") unless COLORMODEL.include?(colormodel)
      Ext.pkl_get_color(@pkl_image, x, y, COLORMODEL[colormodel])
    end
    
    # 特定ピクセルに色をセットする
    def setcolor(x, y, colormodel, color)
      nil unless(@pkl_image)
      validate_position(x, y)
      error("invalid parameter. # => #{colormodel}") unless COLORMODEL.include?(colormodel)
      validate_numeric(color)
      error("invalid parameter. # => #{color}") unless color>=0 && color<=255
      Ext.pkl_set_color(@pkl_image, x, y, COLORMODEL[colormodel], color)
    end
    
    # 特定ピクセルの色を取り出す
    def getpixel(x, y)
      return unless @pkl_image
      validate_position(x, y)
      color = Ext.pkl_get_pixel(self.rawdata, x, y)
      
      case self.colorspace
        when :GRAY
          v = { 'gray' => Ext.pkl_color(color, COLORMODEL[:gray]) }
        when :RGB
          v = {
            'red'   => Ext.pkl_color(color, COLORMODEL[:red]),
            'green' => Ext.pkl_color(color, COLORMODEL[:green]),
            'blue'  => Ext.pkl_color(color, COLORMODEL[:blue]) }
        when :CMYK
          v = {
            'cyan'    => Ext.pkl_color(color, COLORMODEL[:cyan]),
            'magenta' => Ext.pkl_color(color, COLORMODEL[:magenta]),
            'yellow'  => Ext.pkl_color(color, COLORMODEL[:yellow]),
            'black'   => Ext.pkl_color(color, COLORMODEL[:black]) }
        else
          v=nil
      end
      Ext.pkl_color_close(color)
      return v
    end

    # 特定ピクセルに色を設定する
    def setpixel(x, y, color, auto=true)
      return unless @pkl_image
      validate_position(x, y)
      error("invalid parameter. # => #{color}") if color.nil?
      error("invalid parameter. # => #{color}") if color.class.name != 'Pikl::Color'
      Ext.pkl_set_pixel(@pkl_image, x, y, color.rawdata)
      color.close if(auto)
    end
    
    def validate_position(x, y)
      validate_numeric(x)
      validate_numeric(y)
      error("value is outside the range.  #{x}") if x<0 || x>=self.width
      error("value is outside the range.  #{y}") if y<0 || y>=self.height
    end
    
  end
end



