#--
# = Pikl - a simple image library.
# Author::    Ryota Maruko and Keiko Soezima
# Copyright:: (c) 2008 Ryota Maruko and Keiko Soezima
# License::   MIT License
#++
$:.unshift(File.dirname(__FILE__)) unless
  $:.include?(File.dirname(__FILE__)) || $:.include?(File.expand_path(File.dirname(__FILE__)))
module Pikl
  require "pikl/const"
  require "pikl/ext"
  require "pikl/errors"
  require "pikl/image"
  require "pikl/color"
  require "pikl/filter"
end
