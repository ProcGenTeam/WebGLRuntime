#include <third_party/imgui/imgui.h>

#include "codegen_utils.h"

#define JSCODEGEN_imguiBind_IMPLEMENTATION
#include "imguiBind.h"

struct ImGuiContext {};

ImGuiContext* js_ImGuiContext_ctor() { return nullptr; }

void js_ImGuiContext_beginWindow(ImGuiContext* _this, const char* title,
                                 Callback* callback) {
  if (ImGui::Begin(title)) {
    (*callback)({});
    ImGui::End();
  }
}

bool js_ImGuiContext_button(ImGuiContext* _this, const char* label) {
  return ImGui::Button(label);
}

void js_ImGuiContext_finalizer(ImGuiContext* val) {}