# frozen_string_literal: true

require 'bundler/gem_tasks'
require 'rspec/core/rake_task'
require 'rubocop/rake_task'

RuboCop::RakeTask.new do |t|
  t.fail_on_error = false
end

RSpec::Core::RakeTask.new(:spec)

task default: :spec
