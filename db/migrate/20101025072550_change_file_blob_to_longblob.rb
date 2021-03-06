class ChangeFileBlobToLongblob < ActiveRecord::Migration
  def self.up
	# カラム型をLONGBLOBに変更
	execute "ALTER TABLE photos MODIFY file LONGBLOB;"
	execute "ALTER TABLE photos MODIFY short_file MEDIUMBLOB;"
  end

  def self.down
	# カラム型をBLOBに戻す
	change_column :photos , :file , :binary
	change_column :photos , :short_file , :binary
  end
end
