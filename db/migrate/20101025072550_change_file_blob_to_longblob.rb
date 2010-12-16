class ChangeFileBlobToLongblob < ActiveRecord::Migration
  def self.up
	# ƒJƒ‰ƒ€Œ^‚ðLONGBLOB‚É•ÏX
	execute "ALTER TABLE photos MODIFY file LONGBLOB;"
	execute "ALTER TABLE photos MODIFY short_file MEDIUMBLOB;"
  end

  def self.down
	# ƒJƒ‰ƒ€Œ^‚ðBLOB‚É–ß‚·
	change_column :photos , :file , :binary
	change_column :photos , :short_file , :binary
  end
end
