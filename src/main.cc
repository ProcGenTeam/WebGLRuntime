#include <quickjs-libc.h>
#include <quickjs.h>

#include <iostream>

#define JSCODEGEN_test_IMPLEMENTATION
#include "test.h"

int js_add(int a, int b) { return a + b; }

void js_puts(const char* str) { puts(str); }

int main(int argc, char* argv[]) {
  JSRuntime* runtime = JS_NewRuntime();
  JSContext* context = JS_NewContext(runtime);

  js_std_add_helpers(context, argc, argv);
  js_init_module_os(context, "os");
  js_init_module_std(context, "std");

  codegen_test_init(context);

  JSValue val;

  const char* test_code = "puts(add(2, 2));";

  val = JS_Eval(context, test_code, sizeof(test_code) / sizeof(char),
                "input.js", 0);

  if (JS_IsException(val)) {
    js_std_dump_error(context);
  }

  JS_FreeValue(context, val);

  JS_FreeContext(context);
  JS_FreeRuntime(runtime);

  return 0;
}