require 'spec_helper'
require 'ffi/agrian_gdal'

RSpec.describe FFI::AgrianGDAL do
  it { should respond_to(:GDALRasterizeLayers_Agrian) }
end
