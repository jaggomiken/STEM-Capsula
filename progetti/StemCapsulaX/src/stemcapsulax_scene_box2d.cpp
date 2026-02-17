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
#include "stemcapsulax_scene_box2d.h"
#include "stemcapsulax_system.h"
#include "stemcapsulax_box2d_proxy.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_b2ddebugdraw.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * MACROS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define STEMCAPSULAX_SCENEB2_ENABLE_TRIGGER_TEST                           0

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * SPECIAL PURPOSE INCLUDE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define RLIGHTS_IMPLEMENTATION
#include <rlights.h>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::SceneBox2D::Impl {
public:
  Impl();
 ~Impl();
 
  Box2DProxy m_proxy;
  RaylibBox2DDebugDraw m_debugdraw;
  Camera2D m_camera;
  Camera3D m_came3d;

   f32 m_fCircleRadius;
  bool m_bDrawCircle;
  bool m_bIncreaseRadius;

   f32 m_fExplosionEnergy;
  bool m_bDrawEnergyRange;
  bool m_bIncreaseEnergy;

  Shader m_shaderCur;
  Light m_light;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::SceneBox2D::SceneBox2D()
: m_pImpl{ nullptr }
{
  m_pImpl = new(std::nothrow) Impl{};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::SceneBox2D::~SceneBox2D()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::SceneBox2D::clear()
{
  m_pImpl->m_proxy.destroyInactiveBodies();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::SceneBox2D::trigger(uint32_t what)
{
#if STEMCAPSULAX_SCENEB2_ENABLE_TRIGGER_TEST == 1  
  f32 ra = 1.5f * (f32(rand()) / f32(RAND_MAX)); // anche 0 !
  f32 x0 = m_pImpl->m_proxy.width() * (f32(rand()) / f32(RAND_MAX));
  m_pImpl->m_proxy.createBodyCircle(x0, 0, ra);
  f32 x1 = m_pImpl->m_proxy.width() * (f32(rand()) / f32(RAND_MAX));
  m_pImpl->m_proxy.createBodyRectangle(x1, 0, 1.0f, 2.0f);
  f32 x2 = m_pImpl->m_proxy.width() * (f32(rand()) / f32(RAND_MAX));
  m_pImpl->m_proxy.createBodyCapsule(x2, 0, {-1.0f, 1.0f}, {2.0f, 3.0f}, .5f);
  f32 x3 = m_pImpl->m_proxy.width() * (f32(rand()) / f32(RAND_MAX));
  b2Vec2 vpoly[] = {
      {  6.0f,   .0f }, {  1.0f,  1.5f }, { -1.0f,  1.5f }, { -2.0f, 1.0f }
    , { -2.0f, -1.0f }, { -1.0f, -1.5f }, {  1.0f, -1.5f }
  };
  m_pImpl->m_proxy.createBodyPolygon(x3, 0
    , vpoly, STEMCAPSULAX_ARRAY_SIZE(vpoly));
#else
  char fn[128];
  std::snprintf(fn, sizeof(fn), "%02u.png", what);
  m_pImpl->m_proxy.createBodyFromImage(imagepath(fn));
#endif    
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::SceneBox2D::show()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::SceneBox2D::draw(RenderTexture2D& rtex
  , const SystemInfo& si)
{
  auto mp = GetMousePosition();
  auto& st = Status::GetInstance();
  auto& cv = Conv::GetInstance();
  bool bDebugDraw = st.data().bDrawDebugEnabled;
  BeginTextureMode(rtex);
    ClearBackground(Fade(BLACK, .0f));
    DrawRectangleLines(0, 0, rtex.texture.width, rtex.texture.height
      , Fade(GRAY, .7f));
    if (m_pImpl->m_bDrawCircle) {
      f32 fr = cv.x_w2s(m_pImpl->m_fCircleRadius);
      DrawCircle(mp.x, mp.y, fr, Fade(GRAY, .6f));
      DrawText(TextFormat("r = %.6f", m_pImpl->m_fCircleRadius)
        , mp.x, mp.y + 12, 10, WHITE);
    }
    if (m_pImpl->m_bDrawEnergyRange) {
      f32 fr = m_pImpl->m_fExplosionEnergy;
      DrawCircle(mp.x, mp.y, fr, Fade(RED, .6f));
      DrawText(TextFormat("E = %.6f", m_pImpl->m_fExplosionEnergy)
        , mp.x, mp.y + 12, 10, RED);
    }
    DrawLine( 0, mp.y, rtex.texture.width, mp.y, Fade(GRAY, .8f));
    DrawLine(mp.x, 0, mp.x, rtex.texture.height, Fade(GRAY, .8f));
    BeginMode2D(m_pImpl->m_camera);
    BeginShaderMode(m_pImpl->m_shaderCur);
    if (bDebugDraw) {
      m_pImpl->m_debugdraw.drawWorld(m_pImpl->m_proxy.worldId());
    }
    EndShaderMode();
    EndMode2D();
  EndTextureMode();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::SceneBox2D::update()
{
  if (IsKeyPressed(KEY_KP_SUBTRACT)) {
    auto mp = GetScreenToWorld2D(GetMousePosition(), m_pImpl->m_camera);
    m_pImpl->m_camera.target = mp;
    m_pImpl->m_camera.offset = GetMousePosition();
    m_pImpl->m_camera.zoom -= .1f;
  }
  if (IsKeyPressed(KEY_KP_ADD)) {
    auto mp = GetScreenToWorld2D(GetMousePosition(), m_pImpl->m_camera);
    m_pImpl->m_camera.target = mp;
    m_pImpl->m_camera.offset = GetMousePosition();
    m_pImpl->m_camera.zoom += .1f;
  }
  
  // Gestisci il tasto 1 per creare ground
  if (IsKeyPressed(KEY_ONE)) {
    auto mp = GetScreenToWorld2D(GetMousePosition(), m_pImpl->m_camera);
    auto& cnv = Conv::GetInstance();
    m_pImpl->m_proxy.createBodyGroundRect(cnv.x_s2w(mp.x), cnv.y_s2w(mp.y)
      , 1.0f, 1.0f);
  }
  
  // Gestisci L-CTRL per impostazione raggio del cerchio
  if (IsKeyDown(KEY_LEFT_CONTROL)) {
    m_pImpl->m_bDrawCircle = true;
    if (m_pImpl->m_bIncreaseRadius) {
      m_pImpl->m_fCircleRadius += (1.0f / GetFPS());
    }
  } else {
    m_pImpl->m_fCircleRadius = .0f;
    m_pImpl->m_bDrawCircle = false;
    m_pImpl->m_bIncreaseRadius = true;
  }

  // Gestisci L-ALT per impostazione energia
  if (IsKeyDown(KEY_LEFT_ALT)) {
    m_pImpl->m_bDrawEnergyRange = true;
    if (m_pImpl->m_bIncreaseEnergy) {
      m_pImpl->m_fExplosionEnergy += 80.0f * (1.0f / GetFPS());
    }
  } else {
    m_pImpl->m_fExplosionEnergy = .0f;
    m_pImpl->m_bDrawEnergyRange = false;
    m_pImpl->m_bIncreaseEnergy = true;
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
      auto mp = GetScreenToWorld2D(GetMousePosition(), m_pImpl->m_camera);
      auto& cnv = Conv::GetInstance();
      m_pImpl->m_proxy.createBodyCircle(cnv.x_s2w(mp.x), cnv.y_s2w(mp.y)
        , m_pImpl->m_fCircleRadius);
      m_pImpl->m_bIncreaseRadius = false;
    }
    if (IsKeyDown(KEY_LEFT_ALT)) {
      auto mp = GetScreenToWorld2D(GetMousePosition(), m_pImpl->m_camera);
      auto& cnv = Conv::GetInstance();
      m_pImpl->m_proxy.createExplosion(cnv.x_s2w(mp.x), cnv.y_s2w(mp.y)
        , m_pImpl->m_fExplosionEnergy);
      m_pImpl->m_bIncreaseEnergy = false;
    }
  }
  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
    auto dt = Vector2Scale(GetMouseDelta(), -1.0f / m_pImpl->m_camera.zoom);
    m_pImpl->m_camera.target = Vector2Add(m_pImpl->m_camera.target, dt);
  }
  
  auto& st = Status::GetInstance();
  if (!st.data().bAppPaused) {
    m_pImpl->m_proxy.update();
  }

  f32 ft = GetFrameTime();
  SetShaderValue(m_pImpl->m_shaderCur
    , GetShaderLocation(m_pImpl->m_shaderCur, "time")
    , &ft, SHADER_UNIFORM_FLOAT);
  auto mp = GetScreenToWorld2D(GetMousePosition(), m_pImpl->m_camera);
  f32 cp3[] = { mp.x, mp.y, .0f };
  SetShaderValue(m_pImpl->m_shaderCur
    , m_pImpl->m_shaderCur.locs[SHADER_LOC_VECTOR_VIEW]
    , cp3, SHADER_UNIFORM_VEC3);
  m_pImpl->m_light.enabled = true;
  m_pImpl->m_light.position = { mp.x, mp.y, 15.0f * cosf(2 * M_PI * ft) };
  UpdateLightValues(m_pImpl->m_shaderCur, m_pImpl->m_light);

  // cleanup bodies
  m_pImpl->m_proxy.removeBodiesYGreaterThan(200.0f);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::SceneBox2D::unshow()
{
  
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::SceneBox2D::Impl::Impl()
: m_fCircleRadius   {   .0f }
, m_bDrawCircle     { false }
, m_bIncreaseRadius {  true }
, m_fExplosionEnergy{ .0f   }
, m_bDrawEnergyRange{ false }
, m_bIncreaseEnergy {  true }
{
  auto& cnv = Conv::GetInstance();
  m_proxy.setWidth ( cnv.fWorldWidth);
  m_proxy.setHeight(cnv.fWorldHeight);
  m_debugdraw.fPixelToUnitRatio =  cnv.fScreenWidth /  cnv.fWorldWidth;
  m_debugdraw.fScreenWidth  = cnv.fScreenWidth;
  m_debugdraw.fScreenHeight = cnv.fScreenHeight;

  f32 cx =  cnv.fScreenWidth / 2.0f;
  f32 cy = cnv.fScreenHeight / 2.0f;

  m_camera.target   = { cx, cy };
  m_camera.offset   = { cx, cy };
  m_camera.rotation = 0.0f;
  m_camera.zoom     = 1.0f;

  m_came3d.position   = {    cx,    cy,-1150.0f };
  m_came3d.target     = {    cx,    cy  ,  0.0f };
  m_came3d.up         = {  0.0f, -1.0f,    0.0f };
  m_came3d.fovy       = 75.0f;
  m_came3d.projection = CAMERA_PERSPECTIVE;

  m_shaderCur = LoadShader(
      shaderpath("raylib_lighting.vs").c_str()
    , shaderpath("raylib_lighting.fs").c_str());
  m_shaderCur.locs[SHADER_LOC_VECTOR_VIEW] =
    GetShaderLocation(m_shaderCur, "viewPos");
  i32 ila = GetShaderLocation(m_shaderCur, "ambient");
  SetShaderValue(m_shaderCur, ila
    , (f32[4]){ .1f, .1f, .1f, 1.0f }
    , SHADER_UNIFORM_VEC4);
  m_light = CreateLight(LIGHT_POINT, { .0f, .0f, .0f }
    , Vector3Zero(), WHITE, m_shaderCur);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::SceneBox2D::Impl::~Impl()
{

}
