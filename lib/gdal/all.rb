# frozen_string_literal: true

Dir[File.join(__dir__, '**/*extensions.rb')].sort.each(&method(:require))
