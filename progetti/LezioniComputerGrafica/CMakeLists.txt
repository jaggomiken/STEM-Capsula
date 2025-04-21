# /* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#  * LCG - LEZIONI COMPUTER GRAFICA
#  * (C) 2025 Copyright by Michele Iacobellis
#  * A project for students...
#  * 
#  * This file is part of LCG.
#  *
#  * LCG is free software: you can redistribute it and/or modify
#  * it under the terms of the GNU General Public License as published by
#  * the Free Software Foundation, either version 3 of the License, or
#  * (at your option) any later version.
#  *
#  * LCG is distributed in the hope that it will be useful,
#  * but WITHOUT ANY WARRANTY; without even the implied warranty of
#  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  * GNU General Public License for more details.
#  *
#  * You should have received a copy of the GNU General Public License
#  * along with LCG. If not, see <http://www.gnu.org/licenses/>.
#  * 
#  * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#
cmake_minimum_required(VERSION 3.22)
project(lcg VERSION 1.0.0 LANGUAGES CXX)
if (NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
endif()
find_package(glm CONFIG REQUIRED)
set(EXT
  ext/imconfig.h 
  ext/imgui.cpp
  ext/imgui.h
  ext/imgui_demo.cpp
  ext/imgui_draw.cpp
  ext/imgui_impl_glfw.cpp
  ext/imgui_impl_glfw.h
  ext/imgui_impl_opengl2.cpp
  ext/imgui_impl_opengl2.h
  ext/imgui_impl_opengl3.cpp
  ext/imgui_impl_opengl3.h
  ext/imgui_impl_opengl3_loader.h
  ext/imgui_internal.h
  ext/imgui_memory_editor.h
  ext/imgui_tables.cpp
  ext/imgui_widgets.cpp
  ext/imstb_rectpack.h
  ext/imstb_textedit.h
  ext/imstb_truetype.h
  ext/TextEditor.h
  ext/TextEditor.cpp
)
set(SRC 
    src/lcg_main.cpp
    src/lcg_common_opengl.cpp 
    src/lcg_common_imgui.cpp 
)
set(INC
    inc/lcg_camera.h
    inc/lcg_common.h
    inc/lcg_priv.h 
    inc/lcg.h
)
set(MOD
  modules/lcg_module_primo.cpp
)
add_executable(lcg ${SRC} ${INC} ${EXT} ${MOD})
target_link_libraries(lcg ${OPENGL_gl_LIBRARY} glfw glm::glm-header-only)
target_include_directories(lcg PUBLIC inc ext ${GLFW_INCLUDE_PATH})
if (EMSCRIPTEN)
    target_compile_options(lcg PUBLIC -O3)
    set_target_properties(lcg PROPERTIES RUNTIME_OUTPUT_NAME "lcg")
    set_target_properties(lcg PROPERTIES SUFFIX ".html")
    target_link_options(lcg PRIVATE --use-port=contrib.glfw3
      --no-heap-copy --embed-file ../data@/ --use-preload-plugins
      -sALLOW_MEMORY_GROWTH=1 -sNO_EXIT_RUNTIME=0 -sASSERTIONS=1
      --shell-file ../emscripten/shell_minimal.html)
endif()
include(GNUInstallDirs)
install(TARGETS lcg RUNTIME DESTINATION bin)
if (WIN32)
  install(FILES 
    data/NovaMono-Regular.ttf
    DESTINATION bin)
else()
  install(FILES 
    data/NovaMono-Regular.ttf
    DESTINATION bin)
endif()
