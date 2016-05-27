require 'ogr'
require_relative '../../ffi/lwgeom'

module OGR
  module GeometryExtensions
    # Methods for {{OGR::Geometry}}s that use Spatialite to operate on
    # themselves.
    module SpatialiteWrappers
      # Uses Spatialite's MakeValid to make the current geometry valid.
      #
      # @return [OGR::Geometry] Returns a new geometry, based on the MakeValid
      #   call.
      def make_valid
        wkt = to_wkt
        geom = FFI::LWGeom.lwgeom_from_wkt(wkt, wkt.length)
        valid_geom = FFI::LWGeom.lwgeom_make_valid(geom)
        valid_wkt = FFI::LWGeom.lwgeom_to_wkt(valid_geom, FFI::LWGeom::VARIANT_WKT_ISO, 16, nil)
        OGR::Geometry.create_from_wkt(valid_wkt)
      end
    end
  end
end

module OGR
  module Geometry
    include OGR::GeometryExtensions::SpatialiteWrappers
  end
end
