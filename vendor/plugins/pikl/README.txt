= pikl

Pikl is an image librarry for JPEG, PNG and Bitmap.
Pikl's API is designed to process the image easily with method chain.

=== Installation of pikl

The simplest way is to install the gem:

  $ sudo gem install pikl
  
To use option for libjpeg and libpng directories:

  $ sudo gem install pikl -- --with-opt-dir=path/to/libraries

== SYNOPSIS:

Basic use of pikl:
  require "rubygems"
  require "pikl"
  Pikl::Image.open('path/to/image.jpg') do |img|
    img.trim(10,5,-10,-5)
    img.save('path/to/output.png', :png)
  end

Use method chain for processing image:
  require "rubygems"
  require "pikl"
  Pikl::Image.open('path/to/image.jpg') do |img|
    img.resize(120,:auto).rotate(90).save('path/to/output.png')
  end


== REQUIREMENTS:

Currently, pikl-core (implemented with C) depends on the following libraries:

* libjpeg
* libpng

notice: pikl use binary library on windows. so these libraries aren't necessary on windows.

== LICENSE:

(The MIT License)

Copyright (c) 2008 pikl.rb Ryota Maruko
Copyright (c) 2008 pikl.so Keiko Soezima

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.