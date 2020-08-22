#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <quickjs-libc.h>
#include <quickjs.h>
#include <string.h>
#include <third_party/imgui/imgui.h>

#include <iostream>

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_sdl.h>

#include "codegen_utils.h"
#include "js_imgui.h"
#include "js_webgl.h"

#define JSCODEGEN_test_IMPLEMENTATION
#include "test.h"

#define JSCODEGEN_core_IMPLEMENTATION
#include "core.h"

// Core Functions

struct Core {
  SDL_Window* window;
  SDL_GLContext glContext;
};

Core* js_Core_ctor() { return nullptr; }

int32_t js_Core_viewportWidth_get(Core* _this) {
  int w = 0, h = 0;

  SDL_GL_GetDrawableSize(_this->window, &w, &h);

  return w;
}

void js_Core_viewportWidth_set(Core* _this, int32_t value) {}

int32_t js_Core_viewportHeight_get(Core* _this) {
  int w = 0, h = 0;

  SDL_GL_GetDrawableSize(_this->window, &w, &h);

  return h;
}

void js_Core_viewportHeight_set(Core* _this, int32_t value) {}

void js_Core_finalizer(Core* val) {}

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

void call_callback(JSContext* context, const char* name, JSValue argument) {
  JSValue global = JS_GetGlobalObject(context);

  JSValue callback_func = JS_GetPropertyStr(context, global, name);

  JSValue value_array[] = {argument};

  if (!JS_IsUndefined(callback_func)) {
    JSValue ret = JS_Call(context, callback_func, global, 1, value_array);

    if (JS_IsException(ret)) {
      js_std_dump_error(context);
    }

    JS_FreeValue(context, ret);
  }

  JS_FreeValue(context, callback_func);
  JS_FreeValue(context, global);
}

void call_draw_function(JSContext* context, JSValue render_wrapper) {
  call_callback(context, "draw", render_wrapper);
}

void call_init_function(JSContext* context, JSValue render_wrapper) {
  call_callback(context, "init", render_wrapper);
}

void call_gui_function(JSContext* context, JSValue imgui_wrapper) {
  call_callback(context, "drawGui", imgui_wrapper);
}

// From: https://www.khronos.org/opengl/wiki/OpenGL_Error
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
    return;
  }

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
  codegen_core_init(context);
  codegen_webgl_init(context);
  codegen_imguiBind_init(context);

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());

    return 1;
  }

  Core core;

  if ((core.window = SDL_CreateWindow(
           "WebGL Runtime", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
           800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)) == nullptr) {
    fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());

    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

  // Based on:
  // https://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php
  core.glContext = SDL_GL_CreateContext(core.window);
  if (core.glContext == nullptr) {
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
  ImGui_ImplSDL2_InitForOpenGL(core.window, core.glContext);
  ImGui_ImplOpenGL3_Init();

  // Setup Core API
  JSValue core_wrapper = js_Core_new(context, &core);

  JSValue global_object = JS_GetGlobalObject(context);

  JS_SetPropertyStr(context, global_object, "core", core_wrapper);

  JS_FreeValue(context, global_object);

  // Load Initaliser Code
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

  ImGuiContext imgui_context;

  JSValue imgui_wrapper = js_ImGuiContext_new(context, &imgui_context);

  // Call Initaliser Function
  call_init_function(context, webgl_context);

  // Main Loop
  bool show_demo_window = true;

  bool done = false;

  while (!done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) done = true;
      if (event.type == SDL_WINDOWEVENT &&
          event.window.event == SDL_WINDOWEVENT_CLOSE &&
          event.window.windowID == SDL_GetWindowID(core.window))
        done = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(core.window);
    ImGui::NewFrame();

    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

    call_gui_function(context, imgui_wrapper);

    ImGui::Render();

    call_draw_function(context, webgl_context);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(core.window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyWindow(core.window);
  SDL_Quit();

  JS_FreeValue(context, webgl_context);

  JS_FreeContext(context);
  JS_FreeRuntime(runtime);

  return 0;
}