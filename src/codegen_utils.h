#pragma once

#include <quickjs.h>

#include <initializer_list>

class Callback {
 private:
  JSContext* _ctx = nullptr;
  JSValue value;

 public:
  Callback(JSContext* ctx, JSValue value) {
    this->_ctx = ctx;

    this->value = JS_DupValue(ctx, value);
  }

  ~Callback() { JS_FreeValue(this->_ctx, this->value); }

  JSValue operator()(std::initializer_list<JSValue> arguments) {
    JSValue global = JS_GetGlobalObject(this->_ctx);

    JSValue ret = JS_Call(this->_ctx, this->value, global, arguments.size(),
                          (JSValue*)arguments.begin());

    JS_FreeValue(this->_ctx, global);

    return ret;
  }
};