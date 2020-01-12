MRuby::Gem::Specification.new('mruby-lrug') do |spec|
  spec.license = 'MIT'
  spec.author = 'Frederick Cheung'
  spec.description = 'LRUG demo'

  spec.add_dependency('mruby-esp32-wifi', :github => 'mruby-esp32/mruby-esp32-wifi')
  spec.add_dependency('mruby-esp32-system', :github => 'mruby-esp32/mruby-esp32-system')
end
