require 'ffi'
require_relative 'gbox'

module FFI
  module LWGeom
    class Geom < FFI::Struct
      layout :type, :uint8_t,
        :flags, :uint8_t,
        :bbox, GBOX.ptr,
        :srid, :int32_t,
        :data, :pointer
    end
  end
end
