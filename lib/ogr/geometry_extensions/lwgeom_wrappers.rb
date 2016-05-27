require 'ogr'
require_relative '../../ffi/lwgeom'

module OGR
  module GeometryExtensions
    # Methods for {{OGR::Geometry}}s that use lwgeom to operate on
    # themselves.
    module LWGeomWrappers
      # Uses lwgeom's MakeValid to make the current geometry valid.
      #
      # @return [OGR::Geometry] Returns a new geometry, based on the
      #   lwgeom_make_valid call.
      def make_valid
        wkt = to_wkt
        geom = FFI::LWGeom.lwgeom_from_wkt(wkt, wkt.length)
        valid_geom = FFI::LWGeom.lwgeom_make_valid(geom)
        precision = 16
        valid_wkt = FFI::LWGeom.lwgeom_to_wkt(valid_geom, FFI::LWGeom::VARIANT_WKT_ISO, precision, nil)

        OGR::Geometry.create_from_wkt(valid_wkt)
      end
    end
  end
end

module OGR
  module Geometry
    include OGR::GeometryExtensions::LWGeomWrappers
  end
end
