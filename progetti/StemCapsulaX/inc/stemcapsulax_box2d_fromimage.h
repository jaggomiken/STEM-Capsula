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
#ifndef stemcapsulax_box2d_fromimage_h
#define stemcapsulax_box2d_fromimage_h

#include "stemcapsulax_system.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  struct Box2DBodyFromImage {
    Box2DBodyFromImage();
    Box2DBodyFromImage(const Box2DBodyFromImage&)              = delete;
    Box2DBodyFromImage(Box2DBodyFromImage&&)                   = delete;
    Box2DBodyFromImage& operator=(const Box2DBodyFromImage&)   = delete;
    Box2DBodyFromImage& operator=(Box2DBodyFromImage&&)        = delete;
   ~Box2DBodyFromImage();

    bool loadImage(const std::string&);
    bool bodyCreate(b2WorldId, f32 offx, f32 offy);

  private:
    struct Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_box2d_fromimage_h
