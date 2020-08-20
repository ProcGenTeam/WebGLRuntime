#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <quickjs-libc.h>
#include <quickjs.h>
#include <string.h>

#include <iostream>

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_sdl.h>

#include "codegen_utils.h"
#include "js_webgl.h"

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

// From: https://www.khronos.org/opengl/wiki/OpenGL_Error
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
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
  codegen_webgl_init(context);

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

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

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

  // Enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  fprintf(stderr, "OpenGL Version: %s\n", glGetString(GL_VERSION));

  // Init IMGUI

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(window, glContext);
  ImGui_ImplOpenGL3_Init();

  JSValue val;

  size_t test_code_length = 0;
  uint8_t* test_code = js_load_file(context, &test_code_length, argv[1]);

  val = JS_Eval(context, (char*)test_code, test_code_length, argv[1], 0);

  if (JS_IsException(val)) {
    fprintf(stderr, "Error: Error running inital script.\n");

    js_std_dump_error(context);

    return 1;
  }

  JS_FreeValue(context, val);

  WebGL2RenderingContext rendering_context;

  JSValue webgl_context =
      js_WebGL2RenderingContext_new(context, &rendering_context);

  call_init_function(context, webgl_context);

  bool show_demo_window = true;

  while (1) {
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        break;
      }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();

    call_draw_function(context, webgl_context);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyWindow(window);
  SDL_Quit();

  JS_FreeValue(context, webgl_context);

  JS_FreeContext(context);
  JS_FreeRuntime(runtime);

  return 0;
}