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

struct WebGLTexture {
  GLuint texture;
  WebGLTexture(GLuint texture) : texture(texture) {}
};

WebGLTexture* js_WebGLTexture_ctor() { return nullptr; }

void js_WebGLTexture_finalizer(WebGLTexture* val) {
  // glDeleteShader(val->shader);
}

struct WebGLVertexArrayObject {
  GLuint vao;
  WebGLVertexArrayObject(GLuint vao) : vao(vao) {}
};

WebGLVertexArrayObject* js_WebGLVertexArrayObject_ctor() { return nullptr; }

void js_WebGLVertexArrayObject_finalizer(WebGLVertexArrayObject* val) {}

struct WebGLUniformLocation {
  GLuint uniform;
  WebGLUniformLocation(GLuint uniform) : uniform(uniform) {}
};

WebGLUniformLocation* js_WebGLUniformLocation_ctor() { return nullptr; }

void js_WebGLUniformLocation_finalizer(WebGLUniformLocation* val) {}

struct WebGL2RenderingContext {};

WebGL2RenderingContext* js_WebGL2RenderingContext_ctor() {
  return new WebGL2RenderingContext();
}

void js_WebGL2RenderingContext_attachShader(WebGL2RenderingContext* _this,
                                            WebGLProgram* program,
                                            WebGLShader* shader) {
  glAttachShader(program->program, shader->shader);
}

void js_WebGL2RenderingContext_bindAttribLocation(WebGL2RenderingContext* _this,
                                                  WebGLProgram* program,
                                                  uint32_t index,
                                                  const char* name) {
  glBindAttribLocation(program->program, index, name);
}

void js_WebGL2RenderingContext_bindBuffer(WebGL2RenderingContext* _this,
                                          uint32_t target,
                                          WebGLBuffer* buffer) {
  if (buffer == nullptr) {
    glBindBuffer(target, NULL);
  } else {
    glBindBuffer(target, buffer->buffer);
  }
}

void js_WebGL2RenderingContext_bindTexture(WebGL2RenderingContext* _this,
                                           uint32_t target,
                                           WebGLTexture* texture) {
  if (texture == nullptr) {
    glBindTexture(target, NULL);
  } else {
    glBindTexture(target, texture->texture);
  }
}

void js_WebGL2RenderingContext_clear(WebGL2RenderingContext* _this,
                                     uint32_t mask) {
  glClear(mask);
}

void js_WebGL2RenderingContext_clearColor(WebGL2RenderingContext* _this,
                                          double red, double green, double blue,
                                          double alpha) {
  glClearColor(red, green, blue, alpha);
}

void js_WebGL2RenderingContext_clearDepth(WebGL2RenderingContext* _this,
                                          double depth) {
  glClearDepth(depth);
}

void js_WebGL2RenderingContext_clearStencil(WebGL2RenderingContext* _this,
                                            int32_t s) {
  glClearStencil(s);
}

void js_WebGL2RenderingContext_colorMask(WebGL2RenderingContext* _this,
                                         bool red, bool green, bool blue,
                                         bool alpha) {
  glColorMask(red, green, blue, alpha);
}

void js_WebGL2RenderingContext_compileShader(WebGL2RenderingContext* _this,
                                             WebGLShader* shader) {
  glCompileShader(shader->shader);
}

WebGLBuffer* js_WebGL2RenderingContext_createBuffer(
    WebGL2RenderingContext* _this) {
  GLuint buf = 0;
  glCreateBuffers(1, &buf);

  return new WebGLBuffer(buf);
}

WebGLProgram* js_WebGL2RenderingContext_createProgram(
    WebGL2RenderingContext* _this) {
  GLuint program = glCreateProgram();

  return new WebGLProgram(program);
}

WebGLShader* js_WebGL2RenderingContext_createShader(
    WebGL2RenderingContext* _this, uint32_t type) {
  GLuint shader = glCreateShader(type);

  return new WebGLShader(shader);
}

WebGLTexture* js_WebGL2RenderingContext_createTexture(
    WebGL2RenderingContext* _this) {
  GLuint tex = 0;
  glGenTextures(1, &tex);

  return new WebGLTexture(tex);
}

void js_WebGL2RenderingContext_cullFace(WebGL2RenderingContext* _this,
                                        uint32_t mode) {
  glCullFace(mode);
}

void js_WebGL2RenderingContext_deleteShader(WebGL2RenderingContext* _this,
                                            WebGLShader* shader) {
  glDeleteShader(shader->shader);
}

void js_WebGL2RenderingContext_depthFunc(WebGL2RenderingContext* _this,
                                         uint32_t func) {
  glDepthFunc(func);
}

void js_WebGL2RenderingContext_depthMask(WebGL2RenderingContext* _this,
                                         bool flag) {
  glDepthMask(flag);
}

void js_WebGL2RenderingContext_disable(WebGL2RenderingContext* _this,
                                       uint32_t cap) {
  glDisable(cap);
}

void js_WebGL2RenderingContext_drawArrays(WebGL2RenderingContext* _this,
                                          uint32_t mode, int32_t first,
                                          int32_t count) {
  glDrawArrays(mode, first, count);
}

void js_WebGL2RenderingContext_drawElements(WebGL2RenderingContext* _this,
                                            uint32_t mode, int32_t count,
                                            uint32_t type, int64_t offset) {
  glDrawElements(mode, count, type, (const void*)offset);
}

void js_WebGL2RenderingContext_enable(WebGL2RenderingContext* _this,
                                      uint32_t cap) {
  glEnable(cap);
}

void js_WebGL2RenderingContext_enableVertexAttribArray(
    WebGL2RenderingContext* _this, uint32_t index) {
  glEnableVertexAttribArray(index);
}

void js_WebGL2RenderingContext_frontFace(WebGL2RenderingContext* _this,
                                         uint32_t mode) {
  glFrontFace(mode);
}

JSValue js_WebGL2RenderingContext_getActiveAttrib(JSContext* ctx,
                                                  WebGL2RenderingContext* _this,
                                                  WebGLProgram* program,
                                                  uint32_t index) {
  char attribName[128];

  GLsizei length = 0;
  GLint size = 0;
  GLenum type = 0;

  glGetActiveAttrib(program->program, index, sizeof(attribName), &length, &size,
                    &type, attribName);

  JSValue obj = JS_NewObject(ctx);

  JS_SetPropertyStr(ctx, obj, "name", JS_NewStringLen(ctx, attribName, length));
  JS_SetPropertyStr(ctx, obj, "type", JS_NewInt32(ctx, type));
  JS_SetPropertyStr(ctx, obj, "size", JS_NewInt32(ctx, size));

  return obj;
}

JSValue js_WebGL2RenderingContext_getActiveUniform(
    JSContext* ctx, WebGL2RenderingContext* _this, WebGLProgram* program,
    uint32_t index) {
  char uniformName[128];

  GLsizei length = 0;
  GLint size = 0;
  GLenum type = 0;

  glGetActiveUniform(program->program, index, sizeof(uniformName), &length,
                     &size, &type, uniformName);

  JSValue obj = JS_NewObject(ctx);

  JS_SetPropertyStr(ctx, obj, "name",
                    JS_NewStringLen(ctx, uniformName, length));
  JS_SetPropertyStr(ctx, obj, "type", JS_NewInt32(ctx, type));
  JS_SetPropertyStr(ctx, obj, "size", JS_NewInt32(ctx, size));

  return obj;
}

int32_t js_WebGL2RenderingContext_getAttribLocation(
    WebGL2RenderingContext* _this, WebGLProgram* program, const char* name) {
  return glGetAttribLocation(program->program, name);
}

uint32_t js_WebGL2RenderingContext_getError(WebGL2RenderingContext* _this) {
  return glGetError();
}

JSValue js_WebGL2RenderingContext_getProgramParameter(
    JSContext* ctx, WebGL2RenderingContext* _this, WebGLProgram* program,
    uint32_t pname) {
  if (pname == GL_LINK_STATUS || pname == GL_VALIDATE_STATUS) {
    int value = 0;
    glGetProgramiv(program->program, pname, &value);

    return value ? JS_TRUE : JS_FALSE;
  } else if (pname == GL_ACTIVE_UNIFORMS || pname == GL_ACTIVE_ATTRIBUTES) {
    int value = 0;
    glGetProgramiv(program->program, pname, &value);

    return JS_NewInt32(ctx, value);
  } else {
    return JS_ThrowTypeError(ctx, "getProgramParamter not implemented for %d",
                             pname);
  }
}

const char* js_WebGL2RenderingContext_getProgramInfoLog(
    WebGL2RenderingContext* _this, WebGLProgram* program) {
  static char infoLog[4096];

  int size = 0;

  glGetProgramInfoLog(program->program, sizeof(infoLog), &size, infoLog);

  return infoLog;
}

JSValue js_WebGL2RenderingContext_getShaderParameter(
    JSContext* ctx, WebGL2RenderingContext* _this, WebGLShader* shader,
    uint32_t pname) {
  if (pname == GL_COMPILE_STATUS) {
    int value = 0;
    glGetShaderiv(shader->shader, pname, &value);

    return value ? JS_TRUE : JS_FALSE;
  } else {
    return JS_ThrowTypeError(ctx, "getShaderParameter not implemented for %d",
                             pname);
  }
}

const char* js_WebGL2RenderingContext_getShaderInfoLog(
    WebGL2RenderingContext* _this, WebGLShader* shader) {
  static char infoLog[4096];

  int size = 0;

  glGetShaderInfoLog(shader->shader, sizeof(infoLog), &size, infoLog);

  return infoLog;
}

const char* js_WebGL2RenderingContext_getShaderSource(
    WebGL2RenderingContext* _this, WebGLShader* shader) {
  static char infoLog[4096];

  int size = 0;

  glGetShaderSource(shader->shader, sizeof(infoLog), &size, infoLog);

  return infoLog;
}

WebGLUniformLocation* js_WebGL2RenderingContext_getUniformLocation(
    WebGL2RenderingContext* _this, WebGLProgram* program, const char* name) {
  GLint location = glGetUniformLocation(program->program, name);

  return new WebGLUniformLocation(location);
}

int64_t js_WebGL2RenderingContext_getVertexAttribOffset(
    WebGL2RenderingContext* _this, uint32_t index, uint32_t pname) {
  return 0;
}

void js_WebGL2RenderingContext_shaderSource(WebGL2RenderingContext* _this,
                                            WebGLShader* shader,
                                            const char* source) {
  size_t sourceLength = strlen(source);

  const char* sourceList[] = {
      source,
  };
  GLsizei sizes[] = {sourceLength};
  glShaderSource(shader->shader, 1, sourceList, sizes);
}

void js_WebGL2RenderingContext_texParameterf(WebGL2RenderingContext* _this,
                                             uint32_t target, uint32_t pname,
                                             double param) {
  glTextureParameterf(target, pname, param);
}

void js_WebGL2RenderingContext_texParameteri(WebGL2RenderingContext* _this,
                                             uint32_t target, uint32_t pname,
                                             int32_t param) {
  glTexParameteri(target, pname, param);
}

void js_WebGL2RenderingContext_uniform1f(WebGL2RenderingContext* _this,
                                         WebGLUniformLocation* location,
                                         double x) {
  glUniform1f(location->uniform, x);
}

void js_WebGL2RenderingContext_uniform2f(WebGL2RenderingContext* _this,
                                         WebGLUniformLocation* location,
                                         double x, double y) {
  glUniform2f(location->uniform, x, y);
}

void js_WebGL2RenderingContext_uniform3f(WebGL2RenderingContext* _this,
                                         WebGLUniformLocation* location,
                                         double x, double y, double z) {
  glUniform3f(location->uniform, x, y, z);
}

void js_WebGL2RenderingContext_uniform4f(WebGL2RenderingContext* _this,
                                         WebGLUniformLocation* location,
                                         double x, double y, double z,
                                         double w) {
  glUniform4f(location->uniform, x, y, z, w);
}

void js_WebGL2RenderingContext_uniform1i(WebGL2RenderingContext* _this,
                                         WebGLUniformLocation* location,
                                         int32_t x) {
  glUniform1i(location->uniform, x);
}

void js_WebGL2RenderingContext_uniform2i(WebGL2RenderingContext* _this,
                                         WebGLUniformLocation* location,
                                         int32_t x, int32_t y) {
  glUniform2i(location->uniform, x, y);
}

void js_WebGL2RenderingContext_uniform3i(WebGL2RenderingContext* _this,
                                         WebGLUniformLocation* location,
                                         int32_t x, int32_t y, int32_t z) {
  glUniform3i(location->uniform, x, y, z);
}

void js_WebGL2RenderingContext_uniform4i(WebGL2RenderingContext* _this,
                                         WebGLUniformLocation* location,
                                         int32_t x, int32_t y, int32_t z,
                                         int32_t w) {
  glUniform4i(location->uniform, x, y, z, w);
}

void js_WebGL2RenderingContext_linkProgram(WebGL2RenderingContext* _this,
                                           WebGLProgram* program) {
  glLinkProgram(program->program);
}

void js_WebGL2RenderingContext_useProgram(WebGL2RenderingContext* _this,
                                          WebGLProgram* program) {
  glUseProgram(program->program);
}

void js_WebGL2RenderingContext_vertexAttribPointer(
    WebGL2RenderingContext* _this, uint32_t index, int32_t size, uint32_t type,
    bool normalized, int32_t stride, int64_t offset) {
  glVertexAttribPointer(index, size, type, normalized, stride, (void*)offset);
}

void js_WebGL2RenderingContext_viewport(WebGL2RenderingContext* _this,
                                        int32_t x, int32_t y, int32_t width,
                                        int32_t height) {
  glViewport(x, y, width, height);
}

void js_WebGL2RenderingContext_bufferData(WebGL2RenderingContext* _this,
                                          uint32_t target, ArrayBufferView data,
                                          uint32_t usage) {
  ArrayBuffer data_buf = data.getBuffer();

  glBufferData(target, data_buf.size, data_buf.arr, usage);

  // for (size_t i = 0; i < data.size; i++) {
  //   fprintf(stderr, "%d,", data.arr[i]);
  // }
  // fprintf(stderr, "\n");
}

void js_WebGL2RenderingContext_texImage2D(WebGL2RenderingContext* _this,
                                          uint32_t target, int32_t level,
                                          int32_t internalformat, int32_t width,
                                          int32_t height, int32_t border,
                                          uint32_t format, uint32_t type,
                                          ArrayBufferView pixels) {
  ArrayBuffer pixels_buf = pixels.getBuffer();

  glTexImage2D(target, level, internalformat, width, height, border, format,
               type, pixels_buf.arr);
}

void js_WebGL2RenderingContext_uniform3fv(WebGL2RenderingContext* _this,
                                          WebGLUniformLocation* location,
                                          ArrayBufferView v) {
  ArrayBuffer value_buf = v.getBuffer();

  glUniform3fv(location->uniform, 1, (float*)value_buf.arr);
}

void js_WebGL2RenderingContext_uniformMatrix4fv(WebGL2RenderingContext* _this,
                                                WebGLUniformLocation* location,
                                                bool transpose,
                                                ArrayBufferView value) {
  ArrayBuffer value_buf = value.getBuffer();

  glUniformMatrix4fv(location->uniform, 1, transpose, (float*)value_buf.arr);
}

WebGLVertexArrayObject* js_WebGL2RenderingContext_createVertexArray(
    WebGL2RenderingContext* _this) {
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);

  return new WebGLVertexArrayObject(vao);
}

void js_WebGL2RenderingContext_bindVertexArray(WebGL2RenderingContext* _this,
                                               WebGLVertexArrayObject* array) {
  if (array == nullptr) {
    glBindVertexArray(NULL);
  } else {
    glBindVertexArray(array->vao);
  }
}

void js_WebGL2RenderingContext_finalizer(WebGL2RenderingContext* val) {
  // delete val;
}
