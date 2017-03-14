require 'ffi/gdal/stuff'

module GDAL
  module DatasetMixins
    # Wrappers for Warp algorithm methods defined in gdal_alg.h.
    module AgrianAlgorithmMethods
      # @param band_numbers [Array<Fixnum>, Fixnum]
      # @param layers [Array<OGR::Layer>, OGR::Layer]
      # @param burn_values [Array<Float>, Float]
      # @param transformer [Proc]
      # @param options [Hash]
      # @option options attribute [String] An attribute field on features to be
      #   used for a burn-in value, which will be burned into all output bands.
      # @option options chunkysize [Fixnum] The height in lines of the chunk to
      #   operate on.
      # @option options all_touched [Boolean]  If +true+, sets all pixels touched
      #   by the line or polygons, not just those whose center is within the
      #   polygon or that are selected by Brezenham's line algorithm.  Defaults to
      #   +false+.
      # @option options burn_value_from ["Z"] Use the Z values of the geometries.
      # @option @options merge_alg [String] "REPLACE" or "ADD".  REPLACE results
      #   in overwriting of value, while ADD adds the new value to the existing
      #   raster, suitable for heatmaps for instance.
      def rasterize_layers_agrian!(band_numbers, layers, burn_values, pixel_divisor = nil,
                            transformer: nil, transform_arg: nil, **options, &progress_block)
        gdal_options = GDAL::Options.pointer(options)
        band_numbers = band_numbers.is_a?(Array) ? band_numbers : [band_numbers]
        log "band numbers: #{band_numbers}"

        # pixel_divisors = band_numbers.is_a?(Array) ? pixel_divisors : [pixel_divisors]
        # log "band numbers: #{pixel_divisors}"

        layers = layers.is_a?(Array) ? layers : [layers]
        log "layers: #{layers}"

        burn_values = burn_values.is_a?(Array) ? burn_values : [burn_values]
        log "burn values: #{burn_values}"

        band_numbers_ptr = FFI::MemoryPointer.new(:pointer, band_numbers.size)
        band_numbers_ptr.write_array_of_int(band_numbers)
        log "band numbers ptr null? #{band_numbers_ptr.null?}"

        layers_ptr = FFI::MemoryPointer.new(:pointer, layers.size)
        layers_ptr.write_array_of_pointer(layers.map(&:c_pointer))
        log "layers ptr null? #{layers_ptr.null?}"

        burn_values_ptr = FFI::MemoryPointer.new(:pointer, burn_values.size)
        burn_values_ptr.write_array_of_double(burn_values)
        log "burn value ptr null? #{burn_values_ptr.null?}"

        # pixel_divisors_ptr = FFI::MemoryPointer.new(:pointer, pixel_divisors.size)
        # pixel_divisors_ptr.write_array_of_double(pixel_divisors)
        # log "burn value ptr null? #{burn_values_ptr.null?}"

        FFI::GDAL::Stuff.GDALRasterizeLayers_Agrian(@c_pointer,      # hDS
                                           band_numbers.size,                                # nBandCount
                                           band_numbers_ptr,                                 # panBandList
                                           layers.size,                                      # nLayerCount
                                           layers_ptr,                                       # pahLayers
                                           transformer,                                      # pfnTransformer
                                           transform_arg,                                    # pTransformerArg
                                           burn_values_ptr,                                  # padfLayerBurnValues
                                           pixel_divisor.to_f,                                    # padfPixelDivisor
                                           gdal_options,                                     # papszOptions
                                           progress_block,                                   # pfnProgress
                                           nil)                                              # pProgressArg
      end
    end
  end
end

module GDAL
  class Dataset
    include GDAL::DatasetMixins::AgrianAlgorithmMethods
  end
end
