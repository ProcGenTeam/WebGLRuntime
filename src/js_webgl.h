#pragma once

#include <quickjs.h>

struct WebGL2RenderingContext {};

JSValue js_WebGL2RenderingContext_new(JSContext* ctx,
                                      WebGL2RenderingContext* user_value);

void codegen_webgl_init(JSContext* ctx);