/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * LCG - LEZIONI COMPUTER GRAFICA
 * (C) 2025 Copyright by Michele Iacobellis
 * A project for students...
 * 
 * This file is part of LCG.
 *
 * LCG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCG. If not, see <http://www.gnu.org/licenses/>.
 * 
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * INCLUDE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * INCLUDE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#ifdef __EMSCRIPTEN__
# include <emscripten/emscripten.h>
# include <emscripten/html5.h>
#endif

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * INCLUDE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#include "lcg.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DEFINIZIONE DI FUNZIONE/METODO
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void MainLoopBody(void* pUserData) 
{
  auto* pAC = reinterpret_cast<lcg::ApplicationContext*>(pUserData);
  if (nullptr == pAC) { return; }

  if (::glfwWindowShouldClose(pAC->pWindow)) { pAC->uAppExit = 1; return; }
  ::glfwPollEvents();

  ::ImGui_ImplOpenGL3_NewFrame();
  ::ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(0, nullptr
    , ImGuiDockNodeFlags_PassthruCentralNode);
  lcg::ImGuiAppModule::RunAll(*pAC);
  ImGui::Render();

  ::glfwGetFramebufferSize(pAC->pWindow, &pAC->iW, &pAC->iH);
  ::glClearColor(pAC->colorclear.r, pAC->colorclear.g
    , pAC->colorclear.b, pAC->colorclear.a);
  ::glClear(GL_COLOR_BUFFER_BIT);
  ::glViewport(0, 0, pAC->iW, pAC->iH);
  lcg::OpenGLAppModule::RunAll(*pAC);

  ::ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ::glfwSwapBuffers(pAC->pWindow);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DEFINIZIONE DI FUNZIONE/METODO
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace lcg {
  void GlfwInitialize(lcg::ApplicationContext* pAC
    , size_t w, size_t h, const std::string& title)
  {
    CHKERRRET(!::glfwInit(), "Problema con GLFW",);
#ifdef __EMSCRIPTEN__
    const char* glsl_version = "#version 300 es";
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2); // 2 for WebGL
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); // 0 for WebGL
    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API); // ES for WebGL
#else
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#endif
    GLFWwindow* window = ::glfwCreateWindow(int(w), int(h)
      , title.c_str(), NULL, NULL);
    CHKERRRET(nullptr == window, "Impossibile creare finestra e contesto",);
    ::glfwMakeContextCurrent(window);
    ::glfwSwapInterval(1);
    ::gladLoadGL(glfwGetProcAddress);
    pAC->pWindow = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "lcg.ini";
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("NovaMono-Regular.ttf", 15.0f * 1.5f);
    io.Fonts->AddFontFromFileTTF("NovaMono-Regular.ttf", 15.0f * 2.0f);
    io.Fonts->AddFontFromFileTTF("NovaMono-Regular.ttf", 15.0f * 1.0f);

    auto& style = ImGui::GetStyle();
    style.FrameRounding   = 3;
    style.ChildRounding   = 3;
    style.WindowRounding  = 3;
    style.ChildBorderSize = 1;
    style.FrameBorderSize = 1;
    style.PopupRounding   = 3;
    style.TabRounding     = 3;
    style.TabBorderSize   = 1;

    ::ImGui_ImplGlfw_InitForOpenGL(pAC->pWindow, true);
#ifdef __EMSCRIPTEN__
    ::ImGui_ImplGlfw_InstallEmscriptenCallbacks(pAC->pWindow, "#canvas");
#endif    
    ::ImGui_ImplOpenGL3_Init(glsl_version);    
  }

  void GlfwShutdown(lcg::ApplicationContext* pAC)
  {
    ::ImGui_ImplOpenGL3_Shutdown();
    ::ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ::glfwDestroyWindow(pAC->pWindow);
    ::glfwTerminate();
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PUNTO DI INGRESSO
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
int main(int argc, char* argv[])
{
  static lcg::ApplicationContext ctx;
  std::printf("[LCG] (C) 2025 by Michele Iacobellis.\n");
  std::printf("[LCG] Applicazione partita...\n");
  double w = 1920, h = 1080;

#ifdef __EMSCRIPTEN__
  emscripten_get_element_css_size("canvas", &w, &h);
#endif  

  lcg::GlfwInitialize(&ctx, int32_t(w), int32_t(h)
    , "Lezioni di Computer Grafica - Michele Iacobellis");
  ctx.colorclear = { 0.2f, 0.3f, 0.4f, 1.0f };

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(MainLoopBody, &ctx, 0, true);
#else
  while (1 != ctx.uAppExit) { MainLoopBody(&ctx); }
#endif

  lcg::GlfwShutdown(&ctx);
  return 0;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * INCLUDE DI IMPLEMENTAZIONE (tecnica specifica)
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
