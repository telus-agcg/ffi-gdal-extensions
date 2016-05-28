require 'ffi-gdal'
require_relative 'lwgeom/geom'

module FFI
  module LWGeom
    extend ::FFI::Library

    # @param [String] lib Name of the library file to find.
    # @return [String] Path to the library file.
    def self.find_lib(lib)
      lib_file_name = "#{lib}.#{FFI::Platform::LIBSUFFIX}*"

      if ENV['LWGEOM_LIBRARY_PATH']
        return File.join(ENV['LWGEOM_LIBRARY_PATH'], lib_file_name)
      end

      FFI::GDAL.search_paths.flat_map do |search_path|
        Dir.glob(search_path).flat_map do |path|
          Dir.glob(File.join(path, lib_file_name))
        end
      end.uniq.first
    end

    ffi_lib [::FFI::CURRENT_PROCESS, find_lib('liblwgeom')]

    VARIANT_WKB_ISO       = 0x01
    VARIANT_WKB_SFSQL     = 0x02
    VARIANT_WKB_EXTENDED  = 0x04
    VARIANT_WKB_NDR       = 0x08
    VARIANT_WKB_XDR       = 0x10
    VARIANT_WKB_HEX       = 0x20

    VARIANT_WKT_ISO       = 0x01
    VARIANT_WKT_SFSQL     = 0x02
    VARIANT_WKT_EXTENDED  = 0x04

    attach_function :lwgeom_from_wkt, [:string, :bool], Geom.ptr
    attach_function :lwgeom_from_wkb, [:pointer, :size_t, :bool], Geom.ptr
    attach_function :lwgeom_to_wkt, [Geom.ptr, :uint8, :int, :pointer], :string
    attach_function :lwgeom_to_wkb, [Geom.ptr, :uint8, :pointer], :pointer

    attach_function :lwgeom_make_valid, [Geom.ptr], Geom.ptr
  end
end