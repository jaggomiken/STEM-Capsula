/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STEMCAPSULAX
 * (C) 2025 Copyright by Michele Iacobellis
 * A project for enjoying computer graphics in C++1x.
 * 
 * This file is part of STEMCAPSULAX.
 *
 * STEMCAPSULAX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * STEMCAPSULAX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with STEMCAPSULAX. If not, see <http://www.gnu.org/licenses/>.
 * 
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#ifndef stemcapsulax_priv_h
#define stemcapsulax_priv_h

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <rlImGui.h>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <mutex>
#include <string>
#include <box2d/box2d.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <TextEditor.h>

#if defined(WIN32)
# define STEM_FILESEP "\\"
#else
# define STEM_FILESEP "/"
#endif

using i08 =   int8_t;
using u08 =  uint8_t;
using i16 =  int16_t;
using u16 = uint16_t;
using i32 =  int32_t;
using u32 = uint32_t;
using i64 =  int64_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;

#define STEMCAPSULAX_ARRAY_SIZE(a)\
  (sizeof(a)/sizeof(a[0]))

#endif // stemcapsulax_priv_h
