require 'ogr/geometry_extensions/lwgeom_wrappers'

RSpec.describe OGR::GeometryExtensions::LWGeomWrappers do
  subject { OGR::Geometry.create_from_wkt(wkt) }

  shared_context 'shared point, no crossing' do
    let(:wkt) do
      %[POLYGON((0 0,0 10,5 5,10 10,10 0,5 5,5 0,0 0))]
    end
  end

  shared_context 'crossing lines' do
    let(:wkt) do
      %[POLYGON((0 0,0 10,10 0,5 0,5 10,0 0))]
    end
  end

  describe '#make_valid' do
    context 'shared point, no crossing' do
      include_context 'shared point, no crossing'

      it 'makes a valid MULTIPOLYGON' do
        result_wkt = 'MULTIPOLYGON (((0 0,0 10,5 5,5 0,0 0)),((5 5,10 10,10 0,5 5)))'
        expect(subject.make_valid.to_wkt).to eq(result_wkt)
      end
    end

    context 'crossing lines' do
      include_context 'crossing lines'

      it 'makes a valid MULTIPOLYGON' do
        result_wkt = 'MULTIPOLYGON (((0 0,0 10,3.333333333333333 6.666666666666667,0 0)),' \
          '((5 5,10 0,5 0,5 5)),' \
          '((5 5,3.333333333333333 6.666666666666667,5 10,5 5)))'
        expect(subject.make_valid.to_wkt).to eq(result_wkt)
      end
    end
  end
end
