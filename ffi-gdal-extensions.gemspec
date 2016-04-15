# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'ffi/gdal/extensions/version'

Gem::Specification.new do |spec|
  spec.name          = 'ffi-gdal-extensions'
  spec.version       = FFI::GDAL::Extensions::VERSION
  spec.authors       = ['Steve Loveless']
  spec.email         = ['steve.loveless@gmail.com']

  spec.summary       = 'Additional functionality to the ffi-gdal gem'
  spec.description   = %(Make ffi-gdal feel more like Ruby, plus adds some new
  functionality)
  spec.homepage = 'https://bitbucket.org/agrian/ffi-gdal-extensions'

  spec.files = `git ls-files -z`.split("\x0").reject do |f|
    f.match(%r{^(test|spec|features)/})
  end
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']

  spec.add_development_dependency 'bundler', '~> 1.11'
  spec.add_development_dependency 'rake', '~> 10.0'
  spec.add_development_dependency 'rspec', '~> 3.0'
end
