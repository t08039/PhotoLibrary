class CreateZiploads < ActiveRecord::Migration
  def self.up
    create_table :ziploads do |t|
      t.string :file_name
      t.binary :file
      t.string :content_type
      t.integer :user_id

      t.timestamps
    end
  end

  def self.down
    drop_table :ziploads
  end
end
