# /* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#  * STEMCAPSULAX
#  * (C) Copyright 2025 by Michele Iacobellis
#  * A project for enjoying computer graphics in C++1x.
#  * 
#  * This file is part of STEMCAPSULAX.
#  *
#  * STEMCAPSULAX is free software: you can redistribute it and/or modify
#  * it under the terms of the GNU General Public License as published by
#  * the Free Software Foundation, either version 3 of the License, or
#  * (at your option) any later version.
#  *
#  * STEMCAPSULAX is distributed in the hope that it will be useful,
#  * but WITHOUT ANY WARRANTY; without even the implied warranty of
#  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  * GNU General Public License for more details.
#  *
#  * You should have received a copy of the GNU General Public License
#  * along with STEMCAPSULAX. If not, see <http://www.gnu.org/licenses/>.
#  * 
#  * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#

cmake_minimum_required(VERSION 3.26)
project(STEMCAPSULAX VERSION 1.0.0 LANGUAGES CXX)
set(TARGET "stemcapsulax")
find_package( OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package( raylib REQUIRED)
find_package(  box2d REQUIRED)
set(SRC 
  src/stemcapsulax_main.cpp 
  src/stemcapsulax_system.cpp
  src/stemcapsulax_gui.cpp
  src/stemcapsulax_hud.cpp
  src/stemcapsulax_scene_manager.cpp
  src/stemcapsulax_scene.cpp
  src/stemcapsulax_scene_layered.cpp
  src/stemcapsulax_scene_box2d.cpp
  src/stemcapsulax_status.cpp
  src/stemcapsulax_box2d_proxy.cpp
  src/stemcapsulax_box2d_fromimage.cpp
  src/stemcapsulax_audio_manager.cpp
  src/stemcapsulax_layer.cpp
  # Add here your files
)
set(INC 
  inc/stemcapsulax_priv.h 
  inc/stemcapsulax_system.h
  inc/stemcapsulax_gui.h
  inc/stemcapsulax_hud.h
  inc/stemcapsulax_scene_manager.h
  inc/stemcapsulax_scene.h
  inc/stemcapsulax_scene_layered.h
  inc/stemcapsulax_scene_box2d.h
  inc/stemcapsulax_status.h
  inc/stemcapsulax_box2d_proxy.h
  inc/stemcapsulax_box2d_fromimage.h
  inc/stemcapsulax_b2ddebugdraw.h
  inc/stemcapsulax_audio_manager.h
  inc/stemcapsulax_layer.h
  # Add here your files
)
set(EXT
  ext/extras/FA6FreeSolidFontData.h
  ext/extras/IconsFontAwesome6.h
  ext/imconfig.h
  ext/imgui_demo.cpp
  ext/imgui_draw.cpp
  ext/imgui_impl_raylib.h
  ext/imgui_internal.h
  ext/imgui_tables.cpp
  ext/imgui_widgets.cpp
  ext/imgui.cpp
  ext/imgui.h
  ext/imstb_rectpack.h
  ext/imstb_textedit.h
  ext/imstb_truetype.h
  ext/rlImGui.cpp
  ext/rlImGui.h
  ext/rlImGuiColors.h
  ext/fft_stereo_simd.h
  ext/TextEditor.h
  ext/TextEditor.cpp
  ext/rlights.h
  ext/TaskScheduler.h
  ext/TaskScheduler.cpp
  ext/LockLessMultiReadPipe.h
)
            add_executable(${TARGET} ${SRC} ${INC} ${EXT})
target_include_directories(${TARGET} PUBLIC inc ext ext/extras)
     target_link_libraries(${TARGET} PRIVATE 
        raylib box2d::box2d Threads::Threads OpenGL::GL)
if (APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET 26.1)
  target_link_libraries(${TARGET} PRIVATE "-framework IOKit")
  target_link_libraries(${TARGET} PRIVATE "-framework Cocoa")
  target_link_libraries(${TARGET} PRIVATE "-framework OpenGL")
elseif (WIN32)
  target_link_libraries(${TARGET} PRIVATE "winmm")
endif()

include(GNUInstallDirs)
install(  TARGETS ${TARGET} RUNTIME DESTINATION bin)
install(DIRECTORY fonts             DESTINATION bin/resources)
install(DIRECTORY shaders           DESTINATION bin/resources)
install(DIRECTORY images            DESTINATION bin/resources)
install(DIRECTORY sounds            DESTINATION bin/resources)
