require 'gdal/dataset_extensions/agrian_algorithm_methods'

RSpec.describe GDAL::Dataset do
  let(:spatial_reference) { OGR::SpatialReference.new_from_epsg 4326 }

  let(:datasource) do
    driver = OGR::Driver.by_name('memory')
    driver.create_data_source 'test'
    # driver = OGR::Driver.by_name('ESRI Shapefile')
    # driver.create_data_source '/tmp/stuff'
  end

  let(:field) { OGR::FieldDefinition.new('z_value', :OFTReal) }

  let(:feature_definition) do
    fd = OGR::FeatureDefinition.new('test FD')
    fd.add_field_definition field
  end

  let(:layer) do
    l = datasource.create_layer 's',
      geometry_type: :wkbMultiPolygon,
      spatial_reference: spatial_reference

    l.create_field field

    l
  end

  let(:features) do
    geometries.map do |geometry, z_value|
      feature = OGR::Feature.new(layer.feature_definition)
      feature.geometry = OGR::Geometry.create_from_wkt geometry
      feature.geometry.spatial_reference = spatial_reference
      feature.set_field_double(0, z_value)
      feature
    end
  end

  let(:dataset) do
    driver = GDAL::Driver.by_name('GTiff')
    driver.create_dataset('/tmp/wat.tiff', 3, 3,
      data_type: :GDT_Float32, band_count: 1).tap do |ds|
      ds.geo_transform = geo_transform
      ds.projection = spatial_reference.to_wkt
    end
  end

  let(:geo_transform) do
    GDAL::GeoTransform.new.tap do |t|
      t.x_origin = 0
      t.y_origin = 0
      t.pixel_width = 1.0
      t.pixel_height = 1.0
    end
  end

  let(:geometries) do
    [
      [build_geom(0, 0, 2, 2), 1],
      [build_geom(0, 3, 2, -2), 2],
      [build_geom(3, 0, -2, 2), 3],
      [build_geom(3, 3, -2, -2), 4]
    ]
  end

  after do
    features.each(&:destroy!)
    datasource.close
    dataset.close
  end

  before do
    build_ogr_datasource
  end

  # describe 'sanity check' do
  #   it 'builds the datasource correctly' do
  #     expect(layer.feature_count).to eq(4)
  #     expect(dataset.raster_bands.length).to eq(1)
  #   end
  # end

  describe '#rasterize_layers_agrian!' do
    context 'algorithm weighted' do
      it 'does things' do
        dataset.rasterize_layers_agrian!(1, layer, 1,
          attribute: 'z_value',
          merge_alg: 'weighted')

        dataset.flush_cache
        expect(dataset.raster_band(1).to_a).to eq([[1.0, 1.0, 6.0], [1.0, 1.0, 6.0], [4.0, 4.0, 16.0]])
      end
    end

    context 'algorithm weighted with divisor' do
      it 'does things' do
        dataset.rasterize_layers_agrian!(1, layer, 1, 0.5,
          attribute: 'z_value',
          merge_alg: 'weighted')

        dataset.flush_cache
        expect(dataset.raster_band(1).to_a).to eq([[0.5, 0.5, 3.0], [0.5, 0.5, 3.0], [2.0, 2.0, 8.0]])
      end
    end

    context 'algorithm weighted' do
      it 'does things' do
        dataset.rasterize_layers_agrian!(1, layer, 1,
          attribute: 'z_value',
          merge_alg: 'replace')

        dataset.flush_cache
        expect(dataset.raster_band(1).to_a).to eq([[1.0, 3.0, 3.0], [2.0, 4.0, 4.0], [2.0, 4.0, 4.0]])
      end
    end

    context 'algorithm add' do
      it 'does things' do
        dataset.rasterize_layers_agrian!(1, layer, 1,
          attribute: 'z_value',
          merge_alg: 'add')

        dataset.flush_cache
        expect(dataset.raster_band(1).to_a).to eq([[1.0, 4.0, 3.0], [3.0, 10.0, 7.0], [2.0, 6.0, 4.0]])
      end
    end

    context 'algorithm max' do
      it 'does things' do
        dataset.rasterize_layers_agrian!(1, layer, 1,
          attribute: 'z_value',
          merge_alg: 'max')

        dataset.flush_cache
        expect(dataset.raster_band(1).to_a).to eq([[1.0, 3.0, 3.0], [2.0, 4.0, 4.0], [2.0, 4.0, 4.0]])
      end
    end
  end

  def build_geom(x, y, width, height)
    coordinates = [
      [x, y],
      [x + width, y],
      [x + width, y + height],
      [x, y + height],
      [x, y]
    ].map { |coordinate| coordinate.join(' ') }.join(',')

    "POLYGON((#{coordinates}))"
  end

  def build_ogr_datasource
    features.each { |feature| layer.create_feature feature }
    layer.sync_to_disk
  end
end
