#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the 
# src/ directory, compile them and link them into lib(subdirectory_name).a 
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

COMPONENT_DEPENDS = mruby_component

COMPONENT_EXTRA_CLEAN := mrb_program.h

main.o: mrb_program.h

mrb_program.h: $(COMPONENT_PATH)/main.rb
	$(COMPONENT_PATH)/../components/mruby_component/mruby/bin/mrbc -B mrb_program -o $@ $^

.PHONY: mrb_program.h
