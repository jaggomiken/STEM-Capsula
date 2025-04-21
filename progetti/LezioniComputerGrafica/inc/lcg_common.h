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
#ifndef lcg_common_h
#define lcg_common_h

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * INCLUDE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#include "lcg_priv.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DICHIARATIVE DI STRUTTURA/CLASSE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace lcg {
  struct Color { float r, g, b, a; };
  struct ApplicationContext {
    GLFWwindow* pWindow;      // puntatore a finestra
      uint32_t  uAppExit;     // se 1, il main loop termina
       int32_t  iW, iH;     // dimensione del canvas
         Color colorclear;    // colore di cancellazione dello sfondo
  };
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DICHIARATIVE DI STRUTTURA/CLASSE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace lcg {
  struct ImGuiAppModule {
    using ModuleCode = std::function<bool(ApplicationContext&)>;
    static void Register(const std::string&, const ModuleCode&);
    static void RunAll(ApplicationContext&);
  };
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DICHIARATIVE DI STRUTTURA/CLASSE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace lcg {
  struct OpenGLAppModule {
    using ModuleCode = std::function<bool(ApplicationContext&)>;
    static void Register(const std::string&, const ModuleCode&);
    static void RunAll(ApplicationContext&);
  };
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DICHIARATIVE DI STRUTTURA/CLASSE/TEMPLATE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace lcg {
  template <typename T> struct AppModuleAutoRegister {
    AppModuleAutoRegister(const std::string& n
      , const typename T::ModuleCode& mc) {
      T::Register(n, mc);
    }
  };
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * MACRO FUNZIONE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define LCG_IMGUI_MODULE_REGISTER(n,m)\
  static lcg::AppModuleAutoRegister<lcg::ImGuiAppModule> M{n, m};
#define LCG_OPENGL_MODULE_REGISTER(n,m)\
  static lcg::AppModuleAutoRegister<lcg::OpenGLAppModule> M{n, m};

#endif // lcg_common_h
