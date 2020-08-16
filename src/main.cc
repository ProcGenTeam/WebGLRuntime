#include <SDL2/SDL.h>
#include <quickjs-libc.h>
#include <quickjs.h>
#include <string.h>

#include <iostream>

#define JSCODEGEN_test_IMPLEMENTATION
#include "test.h"

// Testing Functions

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

// Renderer Functions

SDL_Renderer* js_Renderer_ctor() { return nullptr; }

void js_Renderer_finalizer(SDL_Renderer* val) {}

void js_Renderer_setDrawColor(SDL_Renderer* _this, int r, int g, int b) {
  SDL_SetRenderDrawColor(_this, r, g, b, SDL_ALPHA_OPAQUE);
}

void js_Renderer_fillRect(SDL_Renderer* _this, int x, int y, int w, int h) {
  SDL_Rect rect = {x, y, w, h};

  SDL_RenderFillRect(_this, &rect);
}

// Other Functions

void call_draw_function(JSContext* context, JSValue render_wrapper) {
  JSValue global = JS_GetGlobalObject(context);

  JSValue draw_func = JS_GetPropertyStr(context, global, "draw");

  JSValue value_array[] = {render_wrapper};

  if (!JS_IsUndefined(draw_func)) {
    JSValue ret = JS_Call(context, draw_func, global, 1, value_array);

    if (JS_IsException(ret)) {
      js_std_dump_error(context);
    }

    JS_FreeValue(context, ret);
  }

  JS_FreeValue(context, draw_func);
  JS_FreeValue(context, global);
}

// Entry Point

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("usage webglruntime <filename>");
    return 1;
  }

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

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());

    return 1;
  }

  SDL_Window* window;
  SDL_Renderer* renderer;
  if (SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer) != 0) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());

    return 1;
  }

  SDL_SetWindowTitle(window, "WebGL Runtime");

  JSValue render_wrapper = js_Renderer_new(context, renderer);

  while (1) {
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 10, 10, 10, SDL_ALPHA_OPAQUE);

    SDL_RenderClear(renderer);

    call_draw_function(context, render_wrapper);

    SDL_RenderPresent(renderer);
  }

  JS_FreeValue(context, render_wrapper);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  JS_FreeContext(context);
  JS_FreeRuntime(runtime);

  return 0;
}