=begin
  usage: ruby extconf.rb [options ...]
  configure options:
    --with-jpeg-include=dir
    --with-jpeg-lib=dir
    --with-png-include=dir
    --with-png-lib=dir
=end

require 'mkmf'
require 'rbconfig'

dir_config('jpeg')
dir_config('png')
if have_header('jpeglib.h') && have_library('jpeg') && have_header('png.h') && have_library('png')
  create_makefile('pikl/pikl')
end
