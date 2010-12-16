class ChangeZiploadBlobToLongblob < ActiveRecord::Migration
  def self.up
	# カラム型をLONGBLOBに変更
	execute "ALTER TABLE ziploads MODIFY file LONGBLOB;"
  end

  def self.down
	# カラム型をBLOBに戻す
	change_column :ziploads , :file , :binary
  end
end
