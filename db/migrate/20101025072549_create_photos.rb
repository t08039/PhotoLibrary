class CreatePhotos < ActiveRecord::Migration
  def self.up
    create_table :photos do |t|
      t.integer :id
      t.string :title
      t.string :file_name
      t.binary :file
      t.binary :short_file
      t.string :content_type
      t.text :description
      t.string :tag
      t.integer :user_id
      t.integer :album_id

      t.timestamps
    end
  end

  def self.down
    drop_table :photos
  end
end
