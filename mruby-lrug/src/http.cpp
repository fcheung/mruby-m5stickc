
#include "mruby.h"
#include "mruby/class.h"
#include "mruby/array.h"
#include <Arduino.h>
#include "HTTPClient.h"
#include "mruby/data.h"

#include "M5StickC.h"
#ifdef __cplusplus
extern "C" {
#endif


/* DigiCert Global Root CA*/
const char * cert = \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"\
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"\
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"\
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"\
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"\
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"\
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"\
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"\
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"\
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"\
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"\
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"\
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"\
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"\
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"\
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"\
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"\
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"\
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"\
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"\
"-----END CERTIFICATE-----";

static void http_client_free(mrb_state *state, void *data){
  HTTPClient *client = (HTTPClient*)data;
  delete client;
}

static const struct mrb_data_type http_client_type = {
  "HTTPClient", http_client_free
};

static mrb_value mrb_http_init(mrb_state *mrb, mrb_value self){
  HTTPClient *client = (HTTPClient*)DATA_PTR(self);
  if(client){
    delete client;
  }
  mrb_data_init(self, NULL, &http_client_type);
  client = new HTTPClient();
  mrb_data_init(self, client, &http_client_type);

  return self;
}

static mrb_value mrb_http_set_auth(mrb_state *mrb, mrb_value self){
  HTTPClient *client = (HTTPClient*)DATA_PTR(self);
  char *username, *password;
  mrb_get_args(mrb, "zz", &username, &password);
  client->setAuthorization(username, password);
  return self;
}

static mrb_bool mrb_http_begin(HTTPClient *client, char *url, mrb_bool https){
  if(https){
    return client->begin(url, cert);
  }
  else{
    return client->begin(url);
  }
}

static mrb_value fetch_http_result(mrb_state *mrb, HTTPClient *client, int result_code){
  String result_body;

  mrb_value result = mrb_ary_new(mrb);
  mrb_ary_push(mrb, result, mrb_fixnum_value(result_code));
  if(result_code>0){
    result_body = client->getString();
  }
  else{
    result_body = HTTPClient::errorToString(result_code);
  }
  mrb_ary_push(mrb, result, mrb_str_new_cstr(mrb, result_body.c_str())); 
  client->end();
  return result;
}
static mrb_value mrb_http_get(mrb_state *mrb, mrb_value self){
  HTTPClient *client = (HTTPClient*)DATA_PTR(self);
  char *url = NULL;
  mrb_bool https;
  mrb_get_args(mrb, "zb", &url,&https);

  if(mrb_http_begin(client, url, https)){
    int result_code = client->GET();
    return fetch_http_result(mrb, client, result_code);    
  }else{
    return mrb_str_new_cstr(mrb, "failure");
  }
}


static mrb_value mrb_http_post(mrb_state *mrb, mrb_value self){
  HTTPClient *client = (HTTPClient*)DATA_PTR(self);
  char *url = NULL;
  char *body = NULL;
  char *content_type = NULL;
  mrb_bool https;
  mrb_int body_length=0;
  mrb_get_args(mrb, "zszb", &url, &body, &body_length,&content_type,&https);
  if(mrb_http_begin(client, url, https)){
    client->addHeader("Content-Type", content_type);
    int result_code = client->POST((uint8_t*)body, body_length);
    return fetch_http_result(mrb, client, result_code);    
  }else{
    return mrb_str_new_cstr(mrb, "failure");
  }
}


static mrb_value mrb_wifi_connect(mrb_state *mrb, mrb_value self){
  char *ssid,*pwd;

  mrb_get_args(mrb, "zz", &ssid, &pwd);
  WiFi.begin(ssid, pwd);

  M5.Lcd.print("Connecting to ");
  M5.Lcd.print(ssid);

  while (WiFi.status() != WL_CONNECTED && WiFi.status() != WL_CONNECT_FAILED) {
      delay(500);
      M5.Lcd.print(".");
  }

  if(WiFi.status() == WL_CONNECTED){
      M5.Lcd.print(" Connected!");
  }

  return mrb_bool_value(WiFi.status() == WL_CONNECTED);
}

static mrb_value mrb_wifi_connected_p(mrb_state *mrb, mrb_value self){
  return mrb_bool_value(WiFi.status() == WL_CONNECTED);
}


static mrb_value mrb_pin_write(mrb_state *mrb, mrb_value self){
  mrb_int pin;
  mrb_bool value;
  mrb_get_args(mrb, "ib", &pin, &value);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, value ? HIGH : LOW);

  return mrb_nil_value();
}


void
mrb_mruby_lrug_gem_init(mrb_state *mrb)
{
  struct RClass *http  = mrb_define_class(mrb, "HTTP",  mrb->object_class);
  struct RClass *wifi  = mrb_define_class(mrb, "Wifi",  mrb->object_class);

  struct RClass *pin  = mrb_define_class(mrb, "Pin",  mrb->object_class);


  mrb_define_method(mrb, http, "initialize", mrb_http_init, MRB_ARGS_NONE());
  mrb_define_method(mrb, http, "authorize", mrb_http_set_auth, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, http, "_get", mrb_http_get,  MRB_ARGS_REQ(2));
  mrb_define_method(mrb, http, "_post", mrb_http_post,  MRB_ARGS_REQ(4));

  mrb_define_class_method(mrb, wifi, "connect", mrb_wifi_connect,  MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, wifi, "connected?", mrb_wifi_connected_p,  MRB_ARGS_REQ(2));

  mrb_define_class_method(mrb, pin, "write", mrb_pin_write,  MRB_ARGS_REQ(2));

}

void
mrb_mruby_lrug_gem_final(mrb_state *mrb)
{
}

#ifdef __cplusplus
}
#endif
