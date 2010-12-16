# -*- encoding: utf-8 -*-

class CommentsController < ApplicationController
  # ログイン制御
  before_filter :login_required #, :only=> [:index, :new, :edit, :destroy]
  
  # GET /comments
  # GET /comments.xml
  def index
    @comments = Comment.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @comments }
    end
  end

  # GET /comments/new
  # GET /comments/new.xml
  def new
    @comment = Comment.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @comment }
    end
  end

  def update_comment
    @comment = Comment.new(params[:comment])
	@comment.user_id = current_user.id
    @comment.save
    get_comment
    render :layout => false
  end

  # DELETE /comments/1
  # DELETE /comments/1.xml
  def destroy
    @comment = Comment.find(params[:id])
    if (@comment.user_id || @photo.user_id) == current_user.id
      photo_id = @comment.photo_id
      @comment.destroy
    end
    respond_to do |format|
      format.html { redirect_to(:controller => 'photos', :action => 'index') }
      format.xml  { head :ok }
    end
  end
end

  # コメント取得
  def get_comment
    @comments = Comment.where(:photo_id => @comment.photo_id)
  end
