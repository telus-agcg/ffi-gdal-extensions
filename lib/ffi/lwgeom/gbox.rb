require 'ffi'

module FFI
  module LWGeom
    class GBOX < FFI::Struct
      layout :flags, :uint8,
        :xmin, :double,
        :xmax, :double,
        :ymin, :double,
        :ymax, :double,
        :zmin, :double,
        :zmax, :double,
        :mmin, :double,
        :mmax, :double
    end
  end
end
