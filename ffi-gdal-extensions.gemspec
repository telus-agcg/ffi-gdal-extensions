# frozen_string_literal: true

lib = File.expand_path('lib', __dir__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'ffi/gdal/extensions/version'

Gem::Specification.new do |spec|
  spec.name          = 'ffi-gdal-extensions'
  spec.version       = FFI::GDAL::Extensions::VERSION
  spec.authors       = ['Steve Loveless']
  spec.email         = ['steve.loveless@agrian.com']

  spec.summary       = 'Additional functionality to the ffi-gdal gem'
  spec.description   = %(Make ffi-gdal feel more like Ruby, plus adds some new
  functionality)
  spec.homepage = 'https://github.com/agrian-inc/ffi-gdal-extensions'

  spec.files = `git ls-files -z`.split("\x0").reject do |f|
    f.match(%r{^(test|spec|features)/})
  end
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']

  spec.required_ruby_version = '>= 2.6'
  spec.metadata['rubygems_mfa_required'] = 'true'

  spec.add_dependency 'bindata', '~> 2.0'
  spec.add_dependency 'ffi-gdal', '>= 1.0.0.beta6'
  spec.add_dependency 'numo-narray'

  spec.add_development_dependency 'bundler'
  spec.add_development_dependency 'byebug'
  spec.add_development_dependency 'pry'
  spec.add_development_dependency 'rake'
  spec.add_development_dependency 'rspec', '~> 3.0'
  spec.add_development_dependency 'rspec-github'
  spec.add_development_dependency 'rubocop'
  spec.add_development_dependency 'rubocop-performance'
  spec.add_development_dependency 'simplecov', '~> 0.9'
end
