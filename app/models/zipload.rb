# -*- encoding: utf-8 -*-
class Zipload < ActiveRecord::Base
    
    belongs_to :photo
    
    def zipfile=(zipfile_field)
        @zipfile=zipfile_field
        unless @zipfile.blank?
            self.file_name     =@zipfile.original_filename.gsub(/[^\w._-]/,'')
            self.content_type  =@zipfile.content_type.chomp
            self.file          =@zipfile.read
        end
    end
	
    def validate
        #ファイルが指定されなかった場合
        if @zipfile.blank?
            errors.add(:zipfile, "は必須入力です")
        else
            #ファイル形式が不正な場合
            if @zipfile.content_type !~/application\/zip/
                errors.add(:zipfile , "ファイル形式が不正です")
            end
        end
    end
end
