require 'ogr'
require_relative '../../ffi/lwgeom'

module OGR
  module GeometryExtensions
    # Methods for {{OGR::Geometry}}s that use lwgeom to operate on
    # themselves.
    #
    # @see http://postgis.net/docs/doxygen/2.1/da/de7/liblwgeom_8h_af8d208cf4c0bb7c9f96c75bddc6c498a.html#af8d208cf4c0bb7c9f96c75bddc6c498a
    module LWGeomWrappers
      # Uses lwgeom's MakeValid to make the current geometry valid.
      #
      # @return [OGR::Geometry] Returns a new geometry, based on the
      #   lwgeom_make_valid call.
      def make_valid
        geom = FFI::LWGeom.lwgeom_from_wkb(to_wkb, wkb_size, FFI::LWGeom::LW_PARSER_CHECK_NONE)
        valid_geom = FFI::LWGeom.lwgeom_make_valid(geom)
        valid_wkb_size = FFI::MemoryPointer.new(:size_t)
        valid_wkb_ptr = FFI::LWGeom.lwgeom_to_wkb(valid_geom, FFI::LWGeom::VARIANT_WKB_EXTENDED, valid_wkb_size)
        valid_wkb = valid_wkb_ptr.read_bytes(valid_wkb_size.read_int)

        OGR::Geometry.create_from_wkb(valid_wkb)
      end
    end
  end
end

module OGR
  module Geometry
    include OGR::GeometryExtensions::LWGeomWrappers
  end
end
