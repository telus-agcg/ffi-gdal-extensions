# frozen_string_literal: true

require 'ffi-gdal'
require_relative 'lwgeom/geom'

module FFI
  module LWGeom
    extend ::FFI::Library

    ffi_lib(::FFI::CURRENT_PROCESS, ENV.fetch('LWGEOM_LIBRARY_PATH', 'lwgeom'))

    VARIANT_WKB_ISO       = 0x01
    VARIANT_WKB_SFSQL     = 0x02
    VARIANT_WKB_EXTENDED  = 0x04
    VARIANT_WKB_NDR       = 0x08
    VARIANT_WKB_XDR       = 0x10
    VARIANT_WKB_HEX       = 0x20

    VARIANT_WKT_ISO       = 0x01
    VARIANT_WKT_SFSQL     = 0x02
    VARIANT_WKT_EXTENDED  = 0x04

    attach_function :lwgeom_from_wkt, %i[string bool], Geom.ptr
    attach_function :lwgeom_from_wkb, %i[pointer size_t bool], Geom.ptr
    attach_function :lwgeom_to_wkt, [Geom.ptr, :uint8, :int, :pointer], :string
    attach_function :lwgeom_to_wkb, [Geom.ptr, :uint8, :pointer], :pointer

    attach_function :lwgeom_make_valid, [Geom.ptr], Geom.ptr
  end
end
