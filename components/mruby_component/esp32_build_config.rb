MRuby::Build.new do |conf|
  toolchain :gcc

  [conf.cc, conf.objc, conf.asm].each do |cc|
    cc.command = 'gcc'
    cc.flags = [%w(-g -std=gnu99 -O3 -Wall -Werror-implicit-function-declaration -Wdeclaration-after-statement -Wwrite-strings)]
  end

  [conf.cxx].each do |cxx|
    cxx.command = 'g++'
    cxx.flags = [%w(-g -O3 -Wall -Werror-implicit-function-declaration)]
  end

  conf.linker do |linker|
    linker.command = 'gcc'
    linker.flags = [%w()]
    linker.libraries = %w(m)
    linker.library_paths = []
  end

  conf.archiver do |archiver|
    archiver.command = "ar"
  end

  conf.gembox 'default'
end

MRuby::CrossBuild.new('esp32') do |conf|
  toolchain :gcc

  conf.cc do |cc|
    cc.include_paths << ENV["COMPONENT_INCLUDES"].split(' ')

    cc.flags << '-Wno-maybe-uninitialized'
    cc.flags.flatten!
    cc.flags.collect! { |x| x.gsub('-MP', '') }

    cc.defines << %w(MRB_HEAP_PAGE_SIZE=64)
    cc.defines << %w(KHASH_DEFAULT_SIZE=8)
    cc.defines << %w(MRB_STR_BUF_MIN_SIZE=20)
    cc.defines << %w(MRB_METHOD_T_STRUCT)

    cc.defines << %w(ESP_PLATFORM)
    cc.defines << %w(ESP32)
    cc.defines << %w(M5STICKC)
  end

  conf.cxx do |cxx|
    cxx.include_paths = conf.cc.include_paths.dup
    cxx.flags.flatten!

    cxx.flags.collect! { |x| x.gsub('-MP', '') }

    cxx.defines = conf.cc.defines.dup
  end

  conf.bins = []
  conf.build_mrbtest_lib_only

  conf.gem :core => "mruby-print"
  conf.gem :core => "mruby-compiler"
  conf.gem :core => "mruby-enumerator"

  conf.gem :git => 'https://github.com/mimaki/mruby-stdio'
  conf.gem :git => "https://github.com/fcheung/mruby-stdio-m5stick.git"
  conf.gem :git => 'https://github.com/fcheung/mruby-lcd-m5stack.git'
  conf.gem :git => 'https://github.com/fcheung/mruby-m5stack-button-watcher.git'
end
