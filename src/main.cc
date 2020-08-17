#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <quickjs-libc.h>
#include <quickjs.h>
#include <string.h>

#include <iostream>

#include "codegen_utils.h"

#define JSCODEGEN_test_IMPLEMENTATION
#include "test.h"

// Testing Functions

int js_add(int a, int b) { return a + b; }

void js_puts(const char* str) { puts(str); }

JSValue js_callMe(Callback* callback) { return (*callback)({}); }

int32_t js_testProp_get() { return 42; }

void js_testProp_set(int32_t value) {}

struct TestClass {
  char* name;

  TestClass(const char* name) { this->name = strdup(name); }

  ~TestClass() { free(this->name); }

  void hello() { printf("Hello, %s\n", this->name); }
};

TestClass* js_TestClass_ctor(const char* name) { return new TestClass(name); }

void js_TestClass_finalizer(TestClass* val) { delete val; }

void js_TestClass_hello(TestClass* _this) { _this->hello(); }

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

void call_init_function(JSContext* context, JSValue render_wrapper) {
  JSValue global = JS_GetGlobalObject(context);

  JSValue draw_func = JS_GetPropertyStr(context, global, "init");

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

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());

    return 1;
  }

  SDL_Window* window;
  SDL_GLContext glContext;

  if ((window = SDL_CreateWindow("WebGL Runtime", SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED, 800, 600,
                                 SDL_WINDOW_OPENGL)) == nullptr) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());

    return 1;
  }

  // Based on:
  // https://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php
  glContext = SDL_GL_CreateContext(window);
  if (glContext == nullptr) {
    fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());

    return 1;
  }

  glewExperimental = GL_TRUE;
  GLenum glewError = glewInit();
  if (glewError != GLEW_OK) {
    fprintf(stderr, "Error initializing GLEW! %s\n",
            glewGetErrorString(glewError));

    return 1;
  }

  if (SDL_GL_SetSwapInterval(1) < 0) {
    fprintf(stderr, "Warning: Unable to set VSync! SDL Error: %s\n",
            SDL_GetError());

    return 1;
  }

  JSValue val;

  size_t test_code_length = 0;
  uint8_t* test_code = js_load_file(context, &test_code_length, argv[1]);

  val = JS_Eval(context, (char*)test_code, test_code_length, argv[1], 0);

  if (JS_IsException(val)) {
    js_std_dump_error(context);
  }

  JS_FreeValue(context, val);

  // call_init_function(context, render_wrapper);

  while (1) {
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        break;
      }
    }

    // call_draw_function(context, render_wrapper);
  }

  SDL_DestroyWindow(window);

  SDL_Quit();

  JS_FreeContext(context);
  JS_FreeRuntime(runtime);

  return 0;
}