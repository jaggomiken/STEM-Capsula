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
#include "stemcapsulax_actor_b2d_image.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_box2d_fromimage.h"
#include "stemcapsulax_layer_box2d.h"
#include "stemcapsulax_b2ddebugdraw.h"
#include <vector>
#define _USE_MATH_DEFINES 
#include <cmath>
#include <rlgl.h>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::ActorB2DImage::Impl {
public:
  Impl(b2WorldId wid, const b2Vec2& center, f32 scale);
 ~Impl();

  b2WorldId m_wid;
  b2Vec2 m_center;
  f32 m_fScale;
  std::vector<b2BodyId> vbodies;

  Shader m_shader;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorB2DImage::ActorB2DImage(b2WorldId wid, const b2Vec2& center
  , f32 scale, const std::string& name)
: ActorBox2D  { wid, name }
, m_pImpl     {   nullptr }
{
  m_pImpl = new(std::nothrow) Impl{wid, center, scale};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorB2DImage::~ActorB2DImage()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::ActorB2DImage::createFromImage(const std::string& fn)
{
  Box2DBodyFromImage bfi;
  if (bfi.loadImage(fn)) {
    if (!bfi.bodyCreate(m_pImpl->m_wid
      , m_pImpl->m_center.x, m_pImpl->m_center.y, m_pImpl->vbodies)) {
      std::printf("[ERROR]: Cannot create bodies from image!\n");
      return false;
    }
    std::printf("[ACTORB2DIMAGE]: OK, %zu bodies created.\n"
      , m_pImpl->vbodies.size());
    return true;
  }
  return false;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorB2DImage::behave(u64 what, const std::vector<f32>& v)
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorB2DImage::update()
{
  
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorB2DImage::draw(RenderTexture2D& rtex, Layer& layer)
{
  auto& camera = static_cast<LayerBox2D&>(layer).camera();
  auto p0 = GetScreenToWorld2D({ .0f, .0f }, camera);
  auto p1 = GetScreenToWorld2D({ 
      f32(rtex.texture.width)
    , f32(rtex.texture.height) }, camera);
  BeginShaderMode(m_pImpl->m_shader);
  auto& cnv = Conv::GetInstance();
  const f32 r = cnv.x_w2s(.5f);
  for (const auto& body : m_pImpl->vbodies) {
    b2ShapeId shp[1];
    auto pos = b2Body_GetPosition(body);
    auto isc = b2Body_GetShapeCount(body);
    b2Body_GetShapes(body, shp, 1);
    auto mat = b2Shape_GetSurfaceMaterial(shp[0]);
    auto coo = b2Vec2{ cnv.x_w2s(pos.x), cnv.y_w2s(pos.y) };
    if (  (coo.x >= p0.x) && (coo.x <= p1.x) 
       && (coo.y >= p0.y) && (coo.y <= p1.y)) 
    {
      DrawCircle(coo.x, coo.y, r, RC(mat.customColor));
    }
  }
  EndShaderMode();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorB2DImage::Impl::Impl(b2WorldId wid
  , const b2Vec2& c, f32 s /* scala */)
  : m_wid    { wid }
  , m_center {   c }
  , m_fScale {   s }
{
  m_shader = LoadShader(
      shaderpath("actor_b2d_image_330.vs").c_str()
    , shaderpath("actor_b2d_image_330.fs").c_str());
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorB2DImage::Impl::~Impl()
{
  for (auto& b : vbodies) { b2DestroyBody(b); }
  vbodies.clear();
  UnloadShader(m_shader);
}
