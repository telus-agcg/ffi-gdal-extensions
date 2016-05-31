require 'spec_helper'
require 'ffi/lwgeom'

RSpec.describe FFI::LWGeom do
  describe '.find_lib' do
    context 'non-existant lib' do
      it 'returns nil' do
        expect(described_class.find_lib('meowmeowmeow')).to be_nil
      end
    end

    context 'valid lib' do
      it 'returns a String containing the path to the library' do
        expect(described_class.find_lib('liblwgeom')).to match(/liblwgeom/)
      end
    end
  end
end
