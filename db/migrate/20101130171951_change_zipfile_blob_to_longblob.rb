class ChangeZipfileBlobToLongblob < ActiveRecord::Migration
  def self.up
  # �J�����^��LONGBLOB�ɕύX
  execute "ALTER TABLE ziploads MODIFY file LONGBLOB;"
  end

  def self.down
    change_column :ziploads , :file , :binary
  end
end
