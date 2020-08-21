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

void js_ImGuiContext_text(ImGuiContext* _this, const char* str) {
  ImGui::Text("%s", str);
}

const char* js_ImGuiContext_inputText(ImGuiContext* _this, const char* label,
                                      const char* last) {
  static char text_buffer[4096] = {0};

  strncpy(text_buffer, last, sizeof(text_buffer));

  ImGui::InputText(label, text_buffer, sizeof(text_buffer));

  return text_buffer;
}

double js_ImGuiContext_inputNumber(ImGuiContext* _this, const char* label,
                                   double last) {
  double value = last;
  ImGui::InputDouble(label, &value);
  return value;
}

double js_ImGuiContext_inputSlider(ImGuiContext* _this, const char* label,
                                   double min, double max, double last) {
  double value = last;
  ImGui::SliderScalar(label, ImGuiDataType_Double, &value, &min, &max);
  return value;
}

void js_ImGuiContext_finalizer(ImGuiContext* val) {}