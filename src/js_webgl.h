#pragma once

#include <quickjs.h>

struct WebGLRenderingContext {};

JSValue js_WebGLRenderingContext_new(JSContext* ctx,
                                     WebGLRenderingContext* user_value);

void codegen_webgl_init(JSContext* ctx);