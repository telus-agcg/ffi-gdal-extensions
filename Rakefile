require 'bundler/gem_tasks'
require 'rspec/core/rake_task'
require 'rake/extensiontask'

RSpec::Core::RakeTask.new(:spec)

task default: :spec

# Clean task

clean_files = Dir.glob('ext/**/Makefile*') + Dir.glob('ext/**/*.{o,so') + Dir.glob('lib/**/*.{o,so')

task :clean do
  clean_files.each { |path| rm_rf path }
end

Rake::ExtensionTask.new 'agrian_gdal' do |ext|
  ext.lib_dir = 'lib/agrian_gdal'
end

task spec: :compile
