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
#include "stemcapsulax_box2d_fromimage.h"
#include "stemcapsulax_system.h"
#include <map>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * MACROS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS/STRUCT DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
struct stemcapsulax::Box2DBodyFromImage::Impl {
  Impl() : m_pColors{ nullptr } {}

     Image m_image;
    Color* m_pColors;
  b2BodyId m_bodyid;

  void m_CreateBody(Color shapecolor, f32 density
    , f32 x, f32 y, f32 w, f32 h, b2WorldId);
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Box2DBodyFromImage::Box2DBodyFromImage()
: m_pImpl{ nullptr }
{
  m_pImpl = new(std::nothrow) Impl{};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Box2DBodyFromImage::~Box2DBodyFromImage()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::Box2DBodyFromImage::loadImage(const std::string& filename)
{
  m_pImpl->m_image = LoadImage(filename.c_str());
  if (!IsImageValid(m_pImpl->m_image)) { return false; }
  ImageFormat(&m_pImpl->m_image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  m_pImpl->m_pColors = LoadImageColors(m_pImpl->m_image);
  if (nullptr == m_pImpl->m_pColors) { return false; }
  return true;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::Box2DBodyFromImage::bodyCreate(b2WorldId wid
  , f32 offx, f32 offy)
{
  if (!IsImageValid(m_pImpl->m_image)) { return false; }
  if (nullptr == m_pImpl->m_pColors) { return false; }

  size_t
      w = m_pImpl->m_image.width
    , h = m_pImpl->m_image.height;

  f32 ww = .05f, wh = .05f, spx = .1f, spy = .1f;
  f32 density = .1f;
  f32 wx_ini = offx - ((ww + spx) * w) / 2.0f, wx = wx_ini
    , wy = offy - (wh * h) / 2.0f;

  for (size_t y = 0;y < h;++y) {
    for (size_t x = 0;x < w;++x) {
      Color c = *(m_pImpl->m_pColors + (w * y) + x);
      m_pImpl->m_CreateBody(c, density, wx, wy, ww, wh, wid);
      wx += (ww + spx);
    }
    wy += (wh + spy);
    wx  = wx_ini;
  }

  return true;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Box2DBodyFromImage::Impl::m_CreateBody(Color shapecolor
  , f32 density, f32 x, f32 y, f32 w, f32 h, b2WorldId wid)
{
  f32 bodyw = w, bodyh = h;
  b2BodyDef bodyDef = b2DefaultBodyDef();
  bodyDef.type      = b2_dynamicBody;
  bodyDef.angularDamping = 4.0f;
  bodyDef.position  = b2Vec2{ x, y };
  bodyDef.rotation  = b2MakeRot(.0f * (B2_PI / 180.0f));
    b2BodyId     bodyId = b2CreateBody(wid, &bodyDef);
   b2Polygon dynamicBox = b2MakeBox(bodyw / 2.0f, bodyh / 2.0f);
  b2ShapeDef   shapeDef = b2DefaultShapeDef();
  shapeDef.enableContactEvents = false;
  shapeDef.enableHitEvents = false;
  shapeDef.density = density;
  shapeDef.material.restitution = 0.0f;
  shapeDef.material.friction = 1.0f;
  shapeDef.material.customColor =
      shapecolor.r << 24
    | shapecolor.g << 16
    | shapecolor.b <<  8
    | shapecolor.a;
  b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
}
