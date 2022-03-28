require 'ffi'
require_relative 'gbox'

module FFI
  module LWGeom
    class Geom < FFI::Struct
      layout :type, :uint8,
        :flags, :uint8, # In PostGIS <=2.5.5, this is a u8; in >=3.x, this is a u16.
        :bbox, GBOX.ptr,
        :srid, :int32,
        :data, :pointer
    end
  end
end
