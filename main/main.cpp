#include <M5StickC.h>
#include <M5Display.h>
#include <stdio.h>
#include <stdint.h>
#include "mruby.h"
#include "mruby/dump.h"
#include "mruby/variable.h"
#include "mruby/string.h"
#include "mrb_program.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif


extern "C" int mirb(mrb_state*);

static mrb_value
run_app_mrb(mrb_state *mrb, mrbc_context *context)
{
  mrb_value val = mrb_nil_value();
  val = mrb_load_irep_cxt(mrb, mrb_program, context);

  return val;
}

void mrubyTask(void *pvParameters)
{
  mrb_state *mrb = NULL;
  mrbc_context *context = NULL;
  mrb_value val;
  int ai=0;

  /* Initialize M5Stack */
  M5.begin();
  mrb = mrb_open();

  if (!mrb) {
    printf("mrb_open() failed.\n");
    M5.lcd.print("mrb_open() failed.\n");
    goto ERROR;
  }

  context = mrbc_context_new(mrb);
  ai = mrb_gc_arena_save(mrb);

  if (M5.BtnA.isReleased()) {
    val = run_app_mrb(mrb, context);
    if (mrb->exc) {
      printf("mrb exception\n");
      val = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
      mrb->exc = 0;
    }
    else {
      if (!mrb_nil_p(val)) {
        M5.lcd.print(" => ");
        if (!mrb_string_p(val)) {
          val = mrb_obj_as_string(mrb, val);
        }
      }
    }
    if (!mrb_nil_p(val)) {
      M5.lcd.printf("%s\n", mrb_str_to_cstr(mrb, val));
    }
  }

  /* Enter interactive mode */
  mirb(mrb);
  mrb_gc_arena_restore(mrb, ai);
  mrbc_context_free(mrb, context);

  mrb_close(mrb);

ERROR:
  while (1) {
    micros();
  }
}

extern "C" void app_main()
{
  initArduino();
  xTaskCreatePinnedToCore(mrubyTask, "mrubyTask", 8192, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
}
