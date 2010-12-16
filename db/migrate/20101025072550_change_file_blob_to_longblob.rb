class ChangeFileBlobToLongblob < ActiveRecord::Migration
  def self.up
	# �J�����^��LONGBLOB�ɕύX
	execute "ALTER TABLE photos MODIFY file LONGBLOB;"
	execute "ALTER TABLE photos MODIFY short_file MEDIUMBLOB;"
  end

  def self.down
	# �J�����^��BLOB�ɖ߂�
	change_column :photos , :file , :binary
	change_column :photos , :short_file , :binary
  end
end
