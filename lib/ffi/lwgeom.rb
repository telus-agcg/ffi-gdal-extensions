require 'ffi-gdal'
require_relative 'lwgeom/geom'

module FFI
  module LWGeom
    extend ::FFI::Library

    # @param [String] lib Name of the library file to find.
    # @return [String] Path to the library file.
    def self.find_lib(lib)
      lib_file_name = "#{lib}.#{FFI::Platform::LIBSUFFIX}*"

      if (lwgeom_library_path = ENV['LWGEOM_LIBRARY_PATH'])
        if File.exist?(lwgeom_library_path) && File.file?(lwgeom_library_path)
          return lwgeom_library_path
        end

        result = Dir[File.join(lwgeom_library_path, lib_file_name)].compact

        if (f = result.first)
          return f
        end

        raise "library '#{lib}' not found"
      end

      search_paths.map do |search_path|
        Dir.glob(search_path).map do |path|
          Dir.glob(File.join(path, lib_file_name))
        end
      end.flatten.uniq.first
      # FFI::GDAL.search_paths.flat_map do |search_path|
      #   Dir.glob(search_path).flat_map do |path|
      #     Dir.glob(File.join(path, lib_file_name))
      #   end
      # end.uniq.first
    end

    # @return [Array<String>] List of paths to search for libs in.
    def self.search_paths
      return [ENV['LWGEOM_LIBRARY_PATH']] if ENV['LWGEOM_LIBRARY_PATH']

      @search_paths ||= begin
        paths = ENV['PATH'].split(File::PATH_SEPARATOR)

        unless FFI::Platform.windows?
          paths += %w[/usr/local/{lib64,lib} /opt/local/{lib64,lib} /usr/{lib64,lib} /usr/lib/{x86_64,i386}-linux-gnu]
        end

        paths
      end
    end

    LIB_PATH = find_lib('liblwgeom*').freeze
    ffi_lib [::FFI::CURRENT_PROCESS, LIB_PATH] if LIB_PATH

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
