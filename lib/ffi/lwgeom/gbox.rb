require 'ffi'

module FFI
  module LWGeom
    # In PostGIS <=2.5.5, this is a u8; in >=3.x, this is a u16.
    # typedef :lwflags_t, :uint16

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
