# frozen_string_literal: true

require 'ffi-gdal'
require_relative 'rttopo/geom'

module FFI
  module Rttopo
    extend ::FFI::Library

    # @return [String]
    def self.rttopo_library_path
      @rttopo_library_path ||= ENV.fetch('RTTOPO_LIBRARY_PATH', 'rttopo')
    end

    if rttopo_library_path.nil? || rttopo_library_path.empty?
      raise FFI::GDAL::LibraryNotFound, "Can't find required rttopo library using path: '#{rttopo_library_path}'"
    end

    ffi_lib(rttopo_library_path)

    RTWKB_ISO       = 0x01
    RTWKB_SFSQL     = 0x02
    RTWKB_EXTENDED  = 0x04
    RTWKB_NDR       = 0x08
    RTWKB_XDR       = 0x10
    RTWKB_HEX       = 0x20

    RTWKT_ISO       = 0x01
    RTWKT_SFSQL     = 0x02
    RTWKT_EXTENDED  = 0x04

    attach_function :rtgeom_init, %i[pointer pointer pointer], :pointer
    attach_function :rtgeom_finish, [:pointer], :void
    attach_function :rtgeom_from_wkb, %i[pointer pointer size_t bool], Geom.ptr
    attach_function :rtgeom_to_wkt, [:pointer, Geom.ptr, :uint8, :int, :pointer], :pointer
    attach_function :rtgeom_to_wkb, [:pointer, Geom.ptr, :uint8, :pointer], :pointer
    attach_function :rtfree, %i[pointer pointer], :void

    attach_function :rtgeom_make_valid, [:pointer, Geom.ptr], Geom.ptr
  end
end

def sneks
  require 'ogr'
  require 'pry'
  # create a context. null indicate using default allocators
  ctx = FFI::Rttopo.rtgeom_init FFI::MemoryPointer::NULL, FFI::MemoryPointer::NULL, FFI::MemoryPointer::NULL

  # create some wkb with ogr
  bytes = OGR::Geometry.create_from_wkt('POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))').to_wkb
  invalid_bytes = OGR::Geometry.create_from_wkt('POLYGON((0 0, 1 1, 0 1, 1 0, 0 0))').to_wkb

  # parse wkb with rttopo
  geom = FFI::Rttopo.rtgeom_from_wkb ctx, bytes, bytes.length, true
  invalid_geom = FFI::Rttopo.rtgeom_from_wkb ctx, invalid_bytes, bytes.length, true

  # fixed the rttopo geom with the self-intersection
  fixed_geom = FFI::Rttopo.rtgeom_make_valid ctx, invalid_geom

  # pointer for reading out the size of things
  size_ptr = FFI::MemoryPointer.new :uint, 1
  fixed_size_ptr = FFI::MemoryPointer.new :uint, 1

  # read to string pointer
  geom_wkt_ptr = FFI::Rttopo.rtgeom_to_wkt ctx, geom, FFI::Rttopo::RTWKT_ISO, 12, size_ptr
  fixed_geom_wkt_ptr = FFI::Rttopo.rtgeom_to_wkt ctx, fixed_geom, FFI::Rttopo::RTWKT_ISO, 12, fixed_size_ptr

  # copy string pointer contents to ruby string
  geom_wkt = geom_wkt_ptr.read_string_length size_ptr.read(:int)
  fixed_geom_wkt = fixed_geom_wkt_ptr.read_string_length fixed_size_ptr.read(:int)

  # read geometries to wkb
  byte_ptr = FFI::Rttopo.rtgeom_to_wkb ctx, geom, FFI::Rttopo::RTWKB_ISO, size_ptr
  fixed_byte_ptr = FFI::Rttopo.rtgeom_to_wkb ctx, fixed_geom, FFI::Rttopo::RTWKB_ISO, fixed_size_ptr

  # read wkb from ffi pointers into ruby string
  geom_wkb = byte_ptr.read_bytes size_ptr.read(:int)
  fixed_geom_wkb = fixed_byte_ptr.read_bytes fixed_size_ptr.read(:int)

  puts "geom wkt: #{geom_wkt}"
  puts "fixed geom wkt: #{fixed_geom_wkt}"
  puts "geom wkb: #{geom_wkb.inspect}"
  puts "fixed_geom wkb: #{fixed_geom_wkb.inspect}"

  FFI::Rttopo.rtfree ctx, geom
  FFI::Rttopo.rtfree ctx, invalid_geom
  FFI::Rttopo.rtfree ctx, fixed_geom
  FFI::Rttopo.rtfree ctx, geom_wkt
  FFI::Rttopo.rtfree ctx, fixed_geom_wkt
  FFI::Rttopo.rtfree ctx, byte_ptr
  FFI::Rttopo.rtfree ctx, fixed_byte_ptr

  FFI::Rttopo.rtgeom_finish ctx
end
