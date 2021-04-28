# frozen_string_literal: true

require 'ogr/geometry/container_mixins'

RSpec.describe OGR::GeometryCollection do
  it_behaves_like 'a container geometry'
end

RSpec.describe OGR::MultiPolygon do
  it_behaves_like 'a container geometry'
end

RSpec.describe OGR::MultiLineString do
  it_behaves_like 'a container geometry'
end

RSpec.describe OGR::MultiPoint do
  it_behaves_like 'a container geometry'
end

RSpec.describe OGR::Polygon do
  it_behaves_like 'a container geometry'
end
