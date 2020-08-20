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

struct ArrayBuffer {
  uint8_t* arr;
  size_t size;

  ArrayBuffer(uint8_t* arr, size_t size) : arr(arr), size(size) {}
};

class ArrayBufferView {
 private:
  JSContext* _ctx = nullptr;
  JSValue value;

 public:
  size_t offset;
  size_t length;
  size_t bytes_per_element;

  ArrayBufferView(JSContext* ctx, JSValue value, size_t offset, size_t length,
                  size_t bytes_per_element)
      : _ctx(ctx),
        value(value),
        offset(offset),
        length(length),
        bytes_per_element(bytes_per_element) {}

  ~ArrayBufferView() {
    // JS_FreeValue(this->_ctx, this->value);
  }

  ArrayBuffer getBuffer() {
    size_t size = 0;
    uint8_t* arr = JS_GetArrayBuffer(this->_ctx, &size, this->value);

    ArrayBuffer ret(arr, size);

    return ret;
  }
};