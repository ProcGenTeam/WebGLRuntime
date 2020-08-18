#include <GL/glew.h>
#include <string.h>

#include "codegen_utils.h"

#define JSCODEGEN_webgl_IMPLEMENTATION
#include "webgl.h"

struct WebGLObject {};

WebGLObject* js_WebGLObject_ctor() { return nullptr; }

void js_WebGLObject_finalizer(WebGLObject* val) {}

struct WebGLBuffer {
  GLuint buffer;
  WebGLBuffer(GLuint buffer) : buffer(buffer) {}
};

WebGLBuffer* js_WebGLBuffer_ctor() { return nullptr; }

void js_WebGLBuffer_finalizer(WebGLBuffer* val) {
  // GLuint buffers[] = {
  //     val->buffer,
  // };
  // glDeleteBuffers(1, buffers);
}

struct WebGLProgram {
  GLuint program;
  WebGLProgram(GLuint program) : program(program) {}
};

WebGLProgram* js_WebGLProgram_ctor() { return nullptr; }

void js_WebGLProgram_finalizer(WebGLProgram* val) {
  // glDeleteProgram(val->program);
}

struct WebGLShader {
  GLuint shader;
  WebGLShader(GLuint shader) : shader(shader) {}
};

WebGLShader* js_WebGLShader_ctor() { return nullptr; }

void js_WebGLShader_finalizer(WebGLShader* val) {
  // glDeleteShader(val->shader);
}

struct WebGLRenderingContext {};

WebGLRenderingContext* js_WebGLRenderingContext_ctor() {
  return new WebGLRenderingContext();
}

void js_WebGLRenderingContext_attachShader(WebGLRenderingContext* _this,
                                           WebGLProgram* program,
                                           WebGLShader* shader) {
  glAttachShader(program->program, shader->shader);
}

void js_WebGLRenderingContext_bindAttribLocation(WebGLRenderingContext* _this,
                                                 WebGLProgram* program,
                                                 uint32_t index,
                                                 const char* name) {
  glBindAttribLocation(program->program, index, name);
}

void js_WebGLRenderingContext_bindBuffer(WebGLRenderingContext* _this,
                                         uint32_t target, WebGLBuffer* buffer) {
  if (buffer == nullptr) {
    glBindBuffer(target, NULL);
  } else {
    glBindBuffer(target, buffer->buffer);
  }
}

void js_WebGLRenderingContext_clear(WebGLRenderingContext* _this,
                                    uint32_t mask) {
  glClear(mask);
}

void js_WebGLRenderingContext_clearColor(WebGLRenderingContext* _this,
                                         double red, double green, double blue,
                                         double alpha) {
  glClearColor(red, green, blue, alpha);
}

void js_WebGLRenderingContext_compileShader(WebGLRenderingContext* _this,
                                            WebGLShader* shader) {
  glCompileShader(shader->shader);
}

WebGLBuffer* js_WebGLRenderingContext_createBuffer(
    WebGLRenderingContext* _this) {
  GLuint buf = 0;
  glCreateBuffers(1, &buf);

  return new WebGLBuffer(buf);
}

WebGLProgram* js_WebGLRenderingContext_createProgram(
    WebGLRenderingContext* _this) {
  GLuint program = glCreateProgram();

  return new WebGLProgram(program);
}

WebGLShader* js_WebGLRenderingContext_createShader(WebGLRenderingContext* _this,
                                                   uint32_t type) {
  GLuint shader = glCreateShader(type);

  return new WebGLShader(shader);
}

void js_WebGLRenderingContext_disable(WebGLRenderingContext* _this,
                                      uint32_t cap) {
  glDisable(cap);
}

void js_WebGLRenderingContext_drawElements(WebGLRenderingContext* _this,
                                           uint32_t mode, int32_t count,
                                           uint32_t type, int64_t offset) {
  glDrawElements(mode, count, type, (const void*)offset);
}

void js_WebGLRenderingContext_enable(WebGLRenderingContext* _this,
                                     uint32_t cap) {
  glEnable(cap);
}

void js_WebGLRenderingContext_enableVertexAttribArray(
    WebGLRenderingContext* _this, uint32_t index) {
  glEnableVertexAttribArray(index);
}

int32_t js_WebGLRenderingContext_getAttribLocation(WebGLRenderingContext* _this,
                                                   WebGLProgram* program,
                                                   const char* name) {
  return glGetAttribLocation(program->program, name);
}

int64_t js_WebGLRenderingContext_getVertexAttribOffset(
    WebGLRenderingContext* _this, uint32_t index, uint32_t pname) {
  return 0;
}

void js_WebGLRenderingContext_shaderSource(WebGLRenderingContext* _this,
                                           WebGLShader* shader,
                                           const char* source) {
  size_t sourceLength = strlen(source);

  const char* sourceList[] = {
      source,
  };
  GLsizei sizes[] = {sourceLength};
  glShaderSource(shader->shader, 1, sourceList, sizes);
}

void js_WebGLRenderingContext_linkProgram(WebGLRenderingContext* _this,
                                          WebGLProgram* program) {
  glLinkProgram(program->program);
}

void js_WebGLRenderingContext_useProgram(WebGLRenderingContext* _this,
                                         WebGLProgram* program) {
  glUseProgram(program->program);
}

void js_WebGLRenderingContext_vertexAttribPointer(
    WebGLRenderingContext* _this, uint32_t index, int32_t size, uint32_t type,
    bool normalized, int32_t stride, int64_t offset) {
  glVertexAttribPointer(index, size, type, normalized, stride, (void*)offset);
}

void js_WebGLRenderingContext_viewport(WebGLRenderingContext* _this, int32_t x,
                                       int32_t y, int32_t width,
                                       int32_t height) {
  glViewport(x, y, width, height);
}

void js_WebGLRenderingContext_bufferData(WebGLRenderingContext* _this,
                                         uint32_t target, ArrayBuffer data,
                                         uint32_t usage) {
  glBufferData(target, data.size, data.arr, usage);

  // for (size_t i = 0; i < data.size; i++) {
  //   fprintf(stderr, "%d,", data.arr[i]);
  // }
  // fprintf(stderr, "\n");
}

void js_WebGLRenderingContext_finalizer(WebGLRenderingContext* val) {
  // delete val;
}
