# -*- encoding: utf-8 -*-
require 'zip/zipfilesystem'
require 'fileutils'
require 'pikl'

class ZiploadsController < ApplicationController
  # ログイン制御
  before_filter :login_required, :only=> [:index, :new, :edit, :destroy]
  
  # GET /ziploads
  # GET /ziploads.xml
  def index
    @ziploads = Zipload.all
    
    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @ziploads }
    end
  end

  # GET /ziploads/1
  # GET /ziploads/1.xml
  def show
    @zipload = Zipload.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @zipload }
    end
  end

  # GET /ziploads/new
  # GET /ziploads/new.xml
  def new
    @zipload = Zipload.new

	render :layout => false
  end

  # GET /ziploads/1/edit
  def edit
    @zipload = Zipload.find(params[:id])
  end

  # POST /ziploads
  # POST /ziploads.xml
  def create
    @zipload = Zipload.new(params[:zipload])

    respond_to do |format|
      @zipload.user_id = current_user.id  
      if @zipload.save
      
        # zipのtmpファイルを作成
        File.open("public/ziptmp/#{@zipload.id}.zip",'w+b'){|file|
          file.binmode
          file.write @zipload.file
        }
        
        # zip展開、zipファイルの削除
        unzip("public/ziptmp/#{@zipload.id}.zip","public/ziptmp/#{@zipload.id}")
        FileUtils.rm_r("public/ziptmp/#{@zipload.id}.zip")
        
        # アルバム作成
        @album = Album.new
        @album.title = @zipload.file_name.sub(/\.zip/,'')
        @album.user_id = current_user.id
        @album.save
        
        # 展開したファイルをデータベースに登録
		Dir::glob("public/ziptmp/#{@zipload.id}/**/*.*").each {|f|
		  if File::ftype(f) == "file"
		    $readfile = File.open(f, 'r+b')
		    @photo = Photo.new
		    @photo.file = $readfile.read
		    if  MimeMagic.by_magic($readfile) == "image/jpeg"
				@photo.content_type   =MimeMagic.by_magic($readfile)
		   	    @photo.file_name = File::basename(f,'')
		    	@photo.title = @photo.file_name.gsub(/\.(.+)/, "")
			    
			    img_tmp = Pikl::Image.open(f)
	                  img_tmp.fit(150, 150).save("public/ziptmp/#{@zipload.id}/tmp.jpg")
	                  img_tmp = File.open("public/ziptmp/#{@zipload.id}/tmp.jpg", 'r+b')
		           @photo.short_file = img_tmp.read
			    img_tmp.close
				
			    @photo.user_id = current_user.id
			    @photo.album_id = @album.id
			    @photo.tag = ""
			    @photo.save
			elsif MimeMagic.by_magic($readfile) == "image/png"
				@photo.content_type   =MimeMagic.by_magic($readfile)
		 	    @photo.file_name = File::basename(f,'')
		    	@photo.title = @photo.file_name.gsub(/\.(.+)/, "")
			    
			    image_tag photo.public_filename, :height => image_size[:height], :width => image_size[:width]
			    
			    img_tmp = Pikl::Image.open(f)
	                  img_tmp.fit(150, 150).save("public/ziptmp/#{@zipload.id}/tmp.png")
	                  img_tmp = File.open("public/ziptmp/#{@zipload.id}/tmp.png", 'r+b')
		           @photo.short_file = img_tmp.read
			    img_tmp.close
				
			    @photo.user_id = current_user.id
			    @photo.album_id = @album.id
			    @photo.tag = ""
			    @photo.save
			end
			$readfile.close
		  end
		}

		
		# 作成した一時ファイルとデータベース削除
		FileUtils.rm_r("public/ziptmp/#{@zipload.id}")
    	@zipload.destroy
		
		
	    format.html { redirect_to(photos_url) }
	    format.xml  { head :ok }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @zipload.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /ziploads/1
  # PUT /ziploads/1.xml
  def update
    @zipload = Zipload.find(params[:id])

    respond_to do |format|
      if @zipload.update_attributes(params[:zipload])
        format.html { redirect_to(@zipload, :notice => 'Zipload was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @zipload.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /ziploads/1
  # DELETE /ziploads/1.xml
  def destroy
    @zipload = Zipload.find(params[:id])
    @zipload.destroy

    respond_to do |format|
      format.html { redirect_to(ziploads_url) }
      format.xml  { head :ok }
    end
  end
  
  private
  
  #ZIP展開
  def unzip(src_path, output_path)
    output_path = (output_path + "/").sub("//", "/")
    Zip::ZipInputStream.open(src_path) do |s|
      while f = s.get_next_entry()
        d = File.dirname(f.name)
        FileUtils.makedirs(output_path + d)
        f =  output_path + f.name
        unless f.match(/\/$/)
          File.open(f, "w+b") do |wf|
            wf.puts(s.read())
          end
        end
      end
    end
  end

end
