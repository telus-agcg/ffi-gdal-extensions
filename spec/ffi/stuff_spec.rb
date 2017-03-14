require 'spec_helper'
require 'ffi/gdal/stuff'

RSpec.describe FFI::GDAL::Stuff do
  describe '.find_lib' do
    context 'non-existant lib' do
      it 'does nothing' do
        expect(true).to be
      end
    end
  end
end
