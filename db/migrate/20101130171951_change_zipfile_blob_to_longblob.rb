class ChangeZipfileBlobToLongblob < ActiveRecord::Migration
  def self.up
  # ƒJƒ‰ƒ€Œ^‚ðLONGBLOB‚É•ÏX
  execute "ALTER TABLE ziploads MODIFY file LONGBLOB;"
  end

  def self.down
    change_column :ziploads , :file , :binary
  end
end
