#include <quickjs-libc.h>
#include <quickjs.h>
#include <string.h>

#include <iostream>

#define JSCODEGEN_test_IMPLEMENTATION
#include "test.h"

int js_add(int a, int b) { return a + b; }

void js_puts(const char* str) { puts(str); }

struct TestClass {
  char* name;

  TestClass(const char* name) { this->name = strdup(name); }

  ~TestClass() { free(this->name); }

  void hello() { printf("Hello, %s\n", this->name); }
};

TestClass* js_TestClass_ctor(const char* name) { return new TestClass(name); }

void js_TestClass_finalizer(TestClass* val) { delete val; }

void js_TestClass_hello(TestClass* _this) { _this->hello(); }

int main(int argc, char* argv[]) {
  JSRuntime* runtime = JS_NewRuntime();
  JSContext* context = JS_NewContext(runtime);

  js_std_add_helpers(context, argc, argv);
  js_init_module_os(context, "os");
  js_init_module_std(context, "std");

  codegen_test_init(context);

  JSValue val;

  size_t test_code_length = 0;
  uint8_t* test_code = js_load_file(context, &test_code_length, argv[1]);

  val = JS_Eval(context, (char*)test_code, test_code_length, argv[1], 0);

  if (JS_IsException(val)) {
    js_std_dump_error(context);
  }

  JS_FreeValue(context, val);

  JS_FreeContext(context);
  JS_FreeRuntime(runtime);

  return 0;
}