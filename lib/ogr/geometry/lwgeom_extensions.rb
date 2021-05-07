# frozen_string_literal: true

require 'ogr/geometry'
require_relative '../../ffi/lwgeom'

module OGR
  module Geometry
    # Methods for {{OGR::Geometry}}s that use lwgeom to operate on
    # themselves.
    #
    # @see http://postgis.net/docs/doxygen/2.1/da/de7/liblwgeom_8h_af8d208cf4c0bb7c9f96c75bddc6c498a.html#af8d208cf4c0bb7c9f96c75bddc6c498a
    module LWGeomExtensions
      # Uses lwgeom's MakeValid to make the current geometry valid.
      #
      # @return [OGR::Geometry] Returns a new geometry, based on the
      #   lwgeom_make_valid call.
      def make_valid
        geom = FFI::LWGeom.lwgeom_from_wkb(to_wkb, wkb_size, false)
        valid_geom = FFI::LWGeom.lwgeom_make_valid(geom)
        valid_wkb_size = FFI::MemoryPointer.new(:size_t)
        valid_wkb_ptr = FFI::LWGeom.lwgeom_to_wkb(valid_geom, FFI::LWGeom::VARIANT_WKB_EXTENDED, valid_wkb_size)
        valid_wkb = valid_wkb_ptr.read_bytes(valid_wkb_size.read_int)

        OGR::Geometry.create_from_wkb(valid_wkb)
      end
    end
  end
end

OGR::GeometryCollection.include(OGR::Geometry::LWGeomExtensions)
OGR::GeometryCollection25D.include(OGR::Geometry::LWGeomExtensions)
OGR::LineString.include(OGR::Geometry::LWGeomExtensions)
OGR::LineString25D.include(OGR::Geometry::LWGeomExtensions)
OGR::LinearRing.include(OGR::Geometry::LWGeomExtensions)
OGR::MultiLineString.include(OGR::Geometry::LWGeomExtensions)
OGR::MultiLineString25D.include(OGR::Geometry::LWGeomExtensions)
OGR::MultiPoint.include(OGR::Geometry::LWGeomExtensions)
OGR::MultiPoint25D.include(OGR::Geometry::LWGeomExtensions)
OGR::MultiPolygon.include(OGR::Geometry::LWGeomExtensions)
OGR::MultiPolygon25D.include(OGR::Geometry::LWGeomExtensions)
OGR::Point.include(OGR::Geometry::LWGeomExtensions)
OGR::Point25D.include(OGR::Geometry::LWGeomExtensions)
OGR::Polygon.include(OGR::Geometry::LWGeomExtensions)
OGR::Polygon25D.include(OGR::Geometry::LWGeomExtensions)
