require 'ffi-gdal'

module FFI
  module AgrianGDAL
    extend ::FFI::Library

    ffi_lib File.expand_path('../../agrian_gdal.so', __FILE__)

    attach_function :GDALRasterizeLayers_Agrian,
                    [
                      FFI::GDAL::GDAL.find_type(:GDALDatasetH),
                      :int,
                      :pointer,
                      :int,
                      :pointer,
                      FFI::GDAL::Alg.find_type(:GDALTransformerFunc),
                      :pointer,
                      :pointer,
                      :double,
                      :pointer,
                      FFI::GDAL::GDAL.find_type(:GDALProgressFunc),
                      :pointer
                    ],
                    CPL::Error::CPLErr
  end
end
