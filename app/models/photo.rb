# -*- encoding: utf-8 -*-
require 'zip/zipfilesystem'
require 'fileutils'
require 'pikl'

class Photo < ActiveRecord::Base
    
    # PhotoはUserに属する
    belongs_to :user
    has_many :comments
    
    def imagefile=(imagefile_field)
        @imagefile=imagefile_field
        unless @imagefile.blank?
            self.file_name           =@imagefile.original_filename.gsub(/[^\w._-]/,'')
            self.content_type   =@imagefile.content_type.chomp
            self.file           =@imagefile.read
        end
    end
	
    def validate
        #ファイルが指定されなかった場合
        #if @imagefile.blank? && self.file.blank?
        #    errors.add(:imagefile, "は必須入力です")
       # else
            #ファイル形式が不正な場合
           # if @imagefile.content_type !~/^image\/jpeg/
           #     errors.add(:imagefile , "ファイル形式がJPEG形式ではありません")
         #   end
       # end
    end
end