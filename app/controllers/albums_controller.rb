class AlbumsController < ApplicationController
  
  # ログイン制御
  before_filter :login_required, :only=> [:index, :new, :edit, :destroy]
  
  # GET /albums
  # GET /albums.xml
  def index
    @albums = Album.where(:user_id => current_user.id).all(:order => 'updated_at desc', :select => 'id, user_id, title, created_at, updated_at')
	@albums = @albums.paginate(:page => params[:page], :per_page => 9)
    get_userlist
    get_new_comment
    
    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @albums }
    end
  end

  # GET /albums/1
  # GET /albums/1.xml
  def show
    @album = Album.find(params[:id])
    @photos = Photo.where(:album_id => params[:id]).all(:order => 'updated_at desc', :select => 'id, title, file_name, description, tag, user_id, created_at, updated_at')
	@photos = @photos.paginate(:page => params[:page], :per_page => 9)

	get_userlist
    get_new_comment
    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @album }
    end
  end

  # GET /albums/new
  # GET /albums/new.xml
  def new
    @album = Album.new
    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @album }
    end
  end

  # GET /albums/1/edit
  def edit
    @album = Album.find(params[:id])
  end

  # POST /albums
  # POST /albums.xml
  def create
    @album = Album.new(params[:album])

    respond_to do |format|
      # ユーザIDを保存
      @album.user_id = current_user.id
      if @album.save
        format.html { redirect_to(@album, :notice => 'Album was successfully created.') }
        format.xml  { render :xml => @album, :status => :created, :location => @album }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @album.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /albums/1
  # PUT /albums/1.xml
  def update
    @album = Album.find(params[:id])

    respond_to do |format|
      if @album.update_attributes(params[:album])
        format.html { redirect_to(@album, :notice => 'Album was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @album.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /albums/1
  # DELETE /albums/1.xml
  def destroy
    @album = Album.find(params[:id])
    
    if @album.user_id == current_user.id then
      # 登録されている写真も削除
      @photo = Photo.where(:album_id => params[:id])
      if @photo then
        for photo in @photo
          photo.destroy
        end
      end
      
      @album.destroy
    end
    
    respond_to do |format|
      format.html { redirect_to(photos_url) }
      format.xml  { head :ok }
    end
  end
  
  # 他のユーザフォト表示
  def show_user_album
    if params[:user_id]
    	@albums = Album.where(:user_id => params[:user_id]).all(:order => 'updated_at desc')
    else
        # ユーザIDの指定がなかったとき仮置き
    end
    @albums = @albums.paginate(:page => params[:page], :per_page => 9)
    @username = User.find(params[:user_id])
    get_userlist
    get_new_comment
    
    respond_to do |format|
      format.html # show_user_album.html.erb
      format.xml  { render :xml => @album }
    end
  end
  
  private
  
  # ユーザーリスト取得
  def get_userlist
  	@user_list = User.find :all,:order => 'id asc',:select => 'id, login',:group => 'login'
  end
  # 最新コメント
  def get_new_comment
    @new_comments = Comment.all(:order => 'updated_at desc', :limit => 10)
  end
end
