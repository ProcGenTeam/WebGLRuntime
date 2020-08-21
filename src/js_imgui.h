#pragma once

#include <quickjs.h>

struct ImGuiContext {};

JSValue js_ImGuiContext_new(JSContext* ctx, ImGuiContext* user_value);

void codegen_imguiBind_init(JSContext* ctx);