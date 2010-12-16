# -*- encoding: utf-8 -*-
require 'zip/zipfilesystem'
require 'fileutils'
require 'pikl'

class PhotosController < ApplicationController
  
  # ログイン制御
  before_filter :login_required #, :only=> [:index, :new, :edit, :destroy]
  
  # GET /photos
  # GET /photos.xml
  def index
	@photos = Photo.where(:user_id => current_user.id).all(:order => 'updated_at desc', :select => 'id, title, file_name, description, tag, user_id, created_at, updated_at')
	@photos = @photos.paginate(:page => params[:page], :per_page => 9)
    get_taglist
    get_userlist
    get_new_comment
    
    # 新規登録用？
    @photo = Photo.new
    
    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @photos }
    end
  end
  
  # GET /photos/1
  # GET /photos/1.xml
  def show
    @photo = Photo.find(params[:id])
    @user = User.find(@photo.user_id)
    if @photo.album_id
    	@album = Album.find(@photo.album_id)
	end
	get_userlist
    get_new_comment
	get_comment
	@comment = Comment.new
	
    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @photo }
    end
  end

  # GET /photos/new
  # GET /photos/new.xml
  def new
    @photo = Photo.new
    
	if params[:album_id]
		$album = Album.find(params[:album_id])
	end
	
	render :layout => false
  end

  # GET /photos/1/edit
  def edit
    @photo = Photo.find(params[:id])
  end

  # POST /photos
  # POST /photos.xml
  def create
    @photo = Photo.new(params[:photo])
    
    respond_to do |format|
      # ユーザIDを保存
      @photo.user_id = current_user.id
      
      #@photo.album_id = $album.id
      
      File.open("public/ziptmp/img_#{current_user.id}.jpg",'w+b'){|file|
        file.binmode
        file.write @photo.file
      }
		
		
		
		img_tmp = Pikl::Image.open("public/ziptmp/img_#{current_user.id}.jpg")

	    img_tmp.fit(150, 150).save("public/ziptmp/img_#{current_user.id}.jpg")
		  
		img_tmp = File.open("public/ziptmp/img_#{current_user.id}.jpg", 'r+b')
		@photo.short_file = img_tmp.read
		img_tmp.close
		FileUtils.rm_r("public/ziptmp/img_#{current_user.id}.jpg")
      
        if (@photo.content_type == "image/jpeg") || (@photo.content_type == "image/png")
          @photo.save
	    end
	    format.html { redirect_to(photos_url) }
	    format.xml  { head :ok }
	   end
  end

  # PUT /photos/1
  # PUT /photos/1.xml
  def update
    @photo = Photo.find(params[:id])

    respond_to do |format|
      if @photo.update_attributes(params[:photo])
        format.html { redirect_to(@photo, :notice => '写真が更新されました。') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @photo.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /photos/1
  # DELETE /photos/1.xml
  def destroy
    
    @photo = Photo.find(params[:id])
    if @photo.user_id == current_user.id then
      @photo.destroy
      @comments = Comment.where(:photo_id => params[:id])
    end
	
    respond_to do |format|
      format.html { redirect_to(photos_url) }
      format.xml  { head :ok }
    end
  end
  
  # 写真データを取得
  def get_photo
    @photo = Photo.find(params[:id])
    send_data(@photo.file,     :file_name => @photo.file_name,
                               :type      => @photo.content_type,
                               :disposition => "inline")
  end
  def get_photo_m
    @photo = Photo.find(params[:id])
    send_data(@photo.file,     :file_name => @photo.file_name,
                               :type      => @photo.content_type,
                               :disposition => "inline")
  end
  def get_thumb
    @photo = Photo.find(params[:id])
    send_data(@photo.short_file,:file_name => @photo.file_name,
                               :type      => @photo.content_type,
                               :disposition => "inline")
  end
  # タグ検索
  def tagsearch
    if params[:tag]
    	@photos = Photo.where(:user_id => current_user.id, :tag => params[:tag]).all(:order => 'updated_at desc', :select => 'id, title, file_name, description, tag, user_id, created_at, updated_at')
    else
    	@photos = Photo.where(:user_id => current_user.id).all(:order => 'updated_at desc', :select => 'id, title, file_name, description, tag, user_id, created_at, updated_at')
    end
      @photos = @photos.paginate(:page => params[:page], :per_page => 9)
      get_taglist
      get_userlist
      get_new_comment
    render :action => 'index'
  end
  
  # 他のユーザフォト表示
  def show_user
    if params[:user_id]
    	@photos = Photo.where(:user_id => params[:user_id]).all(:order => 'updated_at desc', :select => 'id, title, file_name, description, tag, user_id, created_at, updated_at')
    else
        # ユーザIDの指定がなかったとき仮置き
    end
    get_userlist
    get_new_comment
    get_taglist_user(params[:user_id])
    @photos = @photos.paginate(:page => params[:page], :per_page => 9)
    @username = User.find(params[:user_id])
    respond_to do |format|
      format.html # show_user.html.erb
      format.xml  { render :xml => @photos }
    end
  end
  
  def test
 	 render :layout => false
  end
  
  def edit_title
    @photo = Photo.find(params[:photo])
    render :layout => false
  end
  
  def update_title
	  photo = params[:photo]
	  @photo = Photo.find(photo[:id])
	  @photo.title = photo[:title]
	  @photo.save
	  render :layout => false
  end
  
  def edit_tag
    @photo = Photo.find(params[:photo])
    render :layout => false
  end
  
  def update_tag
  	  photo = params[:photo]
	  @photo = Photo.find(photo[:id])
	  @photo.tag = photo[:tag]
	  @photo.save
	  render :layout => false
  end
  
  private
  
  # タグリスト取得
  def get_taglist
  	@photo_tags = Photo.where(:user_id => current_user.id).find :all,:select => 'tag',:group => 'tag'
  end
  # 自分以外のユーザ
  def get_taglist_user(tag_user_id)
  	@photo_tags = Photo.where(:user_id => tag_user_id).find :all,:select => 'tag',:group => 'tag'
  end
  
  # ユーザーリスト取得
  def get_userlist
  	@user_list = User.find :all,:order => 'id asc',:select => 'id, login',:group => 'login'
  end
  
  # コメント取得
  def get_comment
    @comments = Comment.where(:photo_id => @photo.id)
  end
  # 最新コメント
  def get_new_comment
    @new_comments = Comment.all(:order => 'updated_at desc', :limit => 10)
  end
end
