require 'ogr'

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
        run_sql %[SELECT MakeValid(#{self_sql})]
      end

      private

      def self_sql
        %[GeomFromText('#{to_wkt}')]
      end

      # @param command [String] The SQL to run.
      # @result [OGR::Geometry]
      def run_sql(command)
        result = nil

        OGR::Driver.by_name('Memory').create_data_source('mem') do |ds|
          layer = ds.execute_sql(command, nil, 'SQLITE')
          feature_count = layer.feature_count
          log "Got #{feature_count} geometries" if feature_count > 1

          feature = layer.next_feature
          result = feature.geometry.dup
          ds.release_result_set(layer)
        end

        result
      end
    end
  end
end

module OGR
  module Geometry
    include OGR::GeometryExtensions::SpatialiteWrappers
  end
end
