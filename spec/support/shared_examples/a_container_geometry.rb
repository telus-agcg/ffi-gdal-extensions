# frozen_string_literal: true

RSpec.shared_examples 'a container geometry' do
  describe '#collection?' do
    it 'returns true' do
      expect(subject.collection?).to eq true
    end
  end
end
