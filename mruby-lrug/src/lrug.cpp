
#include "mruby.h"
#include "mruby/class.h"
#include <Arduino.h>
#include "HTTPClient.h"

#ifdef __cplusplus
extern "C" {
#endif


static mrb_value mrb_http_get(mrb_state *mrb, mrb_value self){
  HTTPClient client;
  char *url = NULL;
  mrb_get_args(mrb, "z", &url);
  client.begin(url);
  client.GET();
  String s = client.getString();
  return mrb_str_new_cstr(mrb, s.c_str());
}


void
mrb_mruby_lrug_gem_init(mrb_state *mrb)
{
  struct RClass *http  = mrb_define_class(mrb, "HTTP",  mrb->object_class);

  mrb_define_class_method(mrb, http, "get", mrb_http_get,  MRB_ARGS_REQ(1));
}

void
mrb_mruby_lrug_gem_final(mrb_state *mrb)
{
}

#ifdef __cplusplus
}
#endif
