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
#include "stemcapsulax_actor_damper.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_layer_box2d.h"
#include "stemcapsulax_b2ddebugdraw.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <rlgl.h>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::ActorDamper::Impl {
public:
  Impl(b2WorldId wid, const b2Vec2& center, f32 scalex, f32 scaley);
 ~Impl();

  b2BodyId m_CreateBox(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& size, f32 dens, Color);
  b2BodyId m_CreateCapsule(b2WorldId wid, f32 x, f32 y
    , const b2Vec2& p0, const b2Vec2& p1, f32 radius, f32 dens, Color);
  b2JointId m_CreateJoint(b2WorldId wid, f32 x, f32 y
    , b2BodyId b0, b2BodyId b1, b2Vec2 angles);

  std::vector<b2BodyId>  vbodies;
  std::vector<b2JointId> vjoints;

  // -----------------------------------------------------------------------
  // Rendering (uniform-based, no instancing) — identico ad ActorPuppet
  // -----------------------------------------------------------------------
  static constexpr size_t SEGS     = 32;  // segmenti per cerchio
  static constexpr size_t MAXPARTS = 8;   // body parts massimi
  static constexpr size_t GEOMMAX  = 512; // vertici geometria totale

  Shader m_shader;
  i32    m_iLocMVP;
  i32    m_iLocPartTransform;
  i32    m_iLocPartColor;
  u32    m_uVAO;
  u32    m_uVBO;

  struct PartGeom { i32 start; i32 count; };
  buffer<PartGeom, MAXPARTS> m_parts;
  buffer<Vector4,  MAXPARTS> m_bufIColors;

  struct geomdata_t {
    buffer<Vector3, GEOMMAX> m_bufPoints;
    buffer<Vector4, GEOMMAX> m_bufColors;
  } m_geomdata;

  void m_BuildGeometry();
  void m_AddCapsuleGeom(const b2Vec2& c1, const b2Vec2& c2, f32 r);
  void m_AddPolygonGeom(const b2Polygon& poly);
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorDamper::ActorDamper(b2WorldId wid, const b2Vec2& center
  , f32 scalex, f32 scaley, const std::string& name)
: ActorBox2D  { wid, name }
, m_pImpl     {   nullptr }
{
  m_pImpl = new(std::nothrow) Impl{wid, center, scalex, scaley};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorDamper::~ActorDamper()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorDamper::behave(u64, const std::vector<f32>& v)
{
  f32 f = v.size() >= 1 ? v.at(0) : 1.0f;
  f32 fscale = 50000.0f, val = -std::logf(f);
  b2Body_ApplyLinearImpulseToCenter(m_pImpl->vbodies[0]
    , { .0f, -val*fscale }, true);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorDamper::update()
{
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorDamper::draw(RenderTexture2D& rtex, Layer& layer)
{
  auto& cnv = Conv::GetInstance();
  BeginShaderMode(m_pImpl->m_shader);

  Matrix matProjection = rlGetMatrixProjection();
  Matrix matModelView  = rlGetMatrixModelview();
  Matrix mvp = MatrixMultiply(matModelView, matProjection);
  SetShaderValueMatrix(m_pImpl->m_shader, m_pImpl->m_iLocMVP, mvp);

  f32 white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  SetShaderValue(m_pImpl->m_shader
    , m_pImpl->m_shader.locs[SHADER_LOC_COLOR_DIFFUSE]
    , white, SHADER_UNIFORM_VEC4);

  rlEnableVertexArray(m_pImpl->m_uVAO);

  size_t k = 0;
  for (const auto& body : m_pImpl->vbodies) {
    const auto& part = m_pImpl->m_parts.itemAt(k);
    if (part.count > 0 && b2Body_IsValid(body)) {
      b2Transform xf = b2Body_GetTransform(body);
      f32 c   = xf.q.c, s = xf.q.s;
      f32 ptx = cnv.x_w2s(xf.p.x);
      f32 pty = cnv.y_w2s(xf.p.y);
      Matrix partTx;
      partTx.m0 =  c;   partTx.m4 = -s;   partTx.m8  = 0.0f; partTx.m12 = ptx;
      partTx.m1 =  s;   partTx.m5 =  c;   partTx.m9  = 0.0f; partTx.m13 = pty;
      partTx.m2 = 0.0f; partTx.m6 = 0.0f; partTx.m10 = 1.0f; partTx.m14 = 0.0f;
      partTx.m3 = 0.0f; partTx.m7 = 0.0f; partTx.m11 = 0.0f; partTx.m15 = 1.0f;
      SetShaderValueMatrix(m_pImpl->m_shader, m_pImpl->m_iLocPartTransform, partTx);
      const auto& col = m_pImpl->m_bufIColors.itemAt(k);
      f32 cv[4] = { col.x, col.y, col.z, col.w };
      SetShaderValue(m_pImpl->m_shader, m_pImpl->m_iLocPartColor
        , cv, SHADER_UNIFORM_VEC4);
      rlDrawVertexArray(part.start, part.count);
    }
    ++k;
  }

  rlDisableVertexArray();
  EndShaderMode();

  // Outline pass (immediate mode) — identico ad ActorPuppet
  {
    static constexpr int SEGS       = 32;
    static constexpr int MAX_SHAPES = 4;
    static constexpr f32 LINE_W     = 1.5f;

    size_t k2 = 0;
    for (const auto& body : m_pImpl->vbodies) {
      if (b2Body_IsValid(body)) {
        b2Transform xf = b2Body_GetTransform(body);

        const auto& cn = m_pImpl->m_bufIColors.itemAt(k2);
        Color oc = ColorFromNormalized({
            std::min(cn.x * 1.8f, 1.0f)
          , std::min(cn.y * 1.8f, 1.0f)
          , std::min(cn.z * 1.8f, 1.0f)
          , cn.w });

        auto ts = [&](b2Vec2 lp) -> Vector2 {
          return { cnv.x_w2s(xf.q.c*lp.x - xf.q.s*lp.y + xf.p.x)
                 , cnv.y_w2s(xf.q.s*lp.x + xf.q.c*lp.y + xf.p.y) };
        };

        b2ShapeId shps[MAX_SHAPES];
        int isc = b2Body_GetShapeCount(body);
        b2Body_GetShapes(body, shps, isc < MAX_SHAPES ? isc : MAX_SHAPES);

        for (int i = 0; i < isc && i < MAX_SHAPES; ++i) {
          auto type = b2Shape_GetType(shps[i]);

          if (type == b2_capsuleShape) {
            auto caps = b2Shape_GetCapsule(shps[i]);
            b2Vec2 c1 = caps.center1, c2 = caps.center2;
            f32 r = caps.radius;
            f32 dx = c2.x - c1.x, dy = c2.y - c1.y;
            f32 len = sqrtf(dx*dx + dy*dy);
            f32 ux  = (len > 0.f) ? dx/len : 0.f;
            f32 uy  = (len > 0.f) ? dy/len : 1.f;
            b2Vec2 A = { c1.x - uy*r, c1.y + ux*r };
            b2Vec2 B = { c1.x + uy*r, c1.y - ux*r };
            b2Vec2 C = { c2.x + uy*r, c2.y - ux*r };
            b2Vec2 D = { c2.x - uy*r, c2.y + ux*r };
            DrawLineEx(ts(A), ts(D), LINE_W, oc);
            DrawLineEx(ts(B), ts(C), LINE_W, oc);
            f32 aa = atan2f(uy, ux);
            for (size_t si = 0; si < SEGS/2; ++si) {
              f32 a0 = aa - f32(PI)/2.f + f32(si  ) * f32(PI) / f32(SEGS/2);
              f32 a1 = aa - f32(PI)/2.f + f32(si+1) * f32(PI) / f32(SEGS/2);
              DrawLineEx(
                  ts({ c2.x + r*cosf(a0), c2.y + r*sinf(a0) })
                , ts({ c2.x + r*cosf(a1), c2.y + r*sinf(a1) })
                , LINE_W, oc);
            }
            for (size_t si = 0; si < SEGS/2; ++si) {
              f32 a0 = aa + f32(PI)/2.f + f32(si  ) * f32(PI) / f32(SEGS/2);
              f32 a1 = aa + f32(PI)/2.f + f32(si+1) * f32(PI) / f32(SEGS/2);
              DrawLineEx(
                  ts({ c1.x + r*cosf(a0), c1.y + r*sinf(a0) })
                , ts({ c1.x + r*cosf(a1), c1.y + r*sinf(a1) })
                , LINE_W, oc);
            }
          }
          else if (type == b2_polygonShape) {
            auto poly = b2Shape_GetPolygon(shps[i]);
            for (int j = 0; j < poly.count; ++j) {
              DrawLineEx(
                  ts(poly.vertices[j])
                , ts(poly.vertices[(j + 1) % poly.count])
                , LINE_W, oc);
            }
          }
        }
      }
      ++k2;
    }
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorDamper::Impl::Impl(b2WorldId wid
  , const b2Vec2& c, f32 sx, f32 sy)
{
  f32 dens = 5.0f;
  auto bidTopPad = m_CreateBox(wid, c.x + sx* .0f, c.y - sy* 4.5f
    , { sx* 6.0f, sy*1.0f }, dens, Color{124, 76, 22, 255});
  auto bidLeDn01 = m_CreateCapsule(wid, c.x - sx* .0f, c.y - sy* 3.0f
    , { sx*-2.5f, sy*-1.5f }, { sx* 2.5f, sy* 1.5f }, sx* .5f, dens, Color{124, 76, 22, 255});
  auto bidLeDn02 = m_CreateCapsule(wid, c.x - sx* .0f, c.y - sy*  .0f
    , { sx*-2.5f, sy*-1.5f }, { sx* 2.5f, sy* 1.5f }, sx* .5f, dens, Color{124, 76, 22, 255});
  auto bidMidPad = m_CreateBox(wid, c.x + sx* .0f, c.y - sy* 1.5f
    , { sx* 6.0f, sy*1.0f }, dens, Color{124, 76, 22, 255});
  auto bidRaUp01 = m_CreateCapsule(wid, c.x - sx* .0f, c.y - sy* 3.0f
    , { sx*-2.5f, sy* 1.5f }, { sx* 2.5f, sy*-1.5f }, sx* .5f, dens, Color{124, 76, 22, 255});
  auto bidRaUp02 = m_CreateCapsule(wid, c.x - sx* .0f, c.y - sy*  .0f
    , { sx*-2.5f, sy* 1.5f }, { sx* 2.5f, sy*-1.5f }, sx* .5f, dens, Color{124, 76, 22, 255});
  auto bidDowPad = m_CreateBox(wid, c.x + sx* .0f, c.y + sy* 1.5f
    , { sx* 6.0f, sy*1.0f }, 200000.0f, Color{124, 76, 22, 255});

  vbodies.push_back(bidTopPad); //  0 pad superiore
  vbodies.push_back(bidLeDn01); //  1 fascia alta da sx a dx verso il basso
  vbodies.push_back(bidRaUp01); //  2 fascia alta da dx a sx verso il basso
  vbodies.push_back(bidMidPad); //  3 pad centrale
  vbodies.push_back(bidLeDn02); //  4 fascia bassa da sx a dx verso il basso
  vbodies.push_back(bidRaUp02); //  5 fascia bassa da dx a sx verso il basso
  vbodies.push_back(bidDowPad); //  6 pad inferiore

  auto jidTPLD1 = m_CreateJoint(wid, c.x - sx*2.5f, c.y - sy* 4.5f, bidLeDn01, bidTopPad, {-0.00f,+0.25f});
  auto jidTPRU1 = m_CreateJoint(wid, c.x + sx*2.5f, c.y - sy* 4.5f, bidTopPad, bidRaUp01, {-0.00f,+0.25f});
  auto jidRU1MD = m_CreateJoint(wid, c.x - sx*2.5f, c.y - sy* 1.5f, bidMidPad, bidRaUp01, {-0.00f,+0.25f});
  auto jidLD1MD = m_CreateJoint(wid, c.x + sx*2.5f, c.y - sy* 1.5f, bidLeDn01, bidMidPad, {-0.00f,+0.25f});
  auto jidLD2MD = m_CreateJoint(wid, c.x - sx*2.5f, c.y - sy* 1.5f, bidLeDn02, bidMidPad, {-0.00f,+0.25f});
  auto jidLD2DN = m_CreateJoint(wid, c.x + sx*2.5f, c.y + sy* 1.5f, bidLeDn02, bidDowPad, {-0.00f,+0.25f});
  auto jidRU2MD = m_CreateJoint(wid, c.x + sx*2.5f, c.y - sy* 1.5f, bidMidPad, bidRaUp02, {-0.00f,+0.25f});
  auto jidRU2DN = m_CreateJoint(wid, c.x - sx*2.5f, c.y + sy* 1.5f, bidDowPad, bidRaUp02, {-0.00f,+0.25f});

  vjoints.push_back(jidTPLD1);
  vjoints.push_back(jidTPRU1);
  vjoints.push_back(jidRU1MD);
  vjoints.push_back(jidLD1MD);
  vjoints.push_back(jidLD2MD);
  vjoints.push_back(jidLD2DN);
  vjoints.push_back(jidRU2MD);
  vjoints.push_back(jidRU2DN);

  m_BuildGeometry();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorDamper::Impl::~Impl()
{
  for (auto& b : vbodies) { if (b2Body_IsValid(b)) { b2DestroyBody(b); }}
  vbodies.clear();
  rlUnloadVertexBuffer(m_uVBO);
  rlUnloadVertexArray(m_uVAO);
  UnloadShader(m_shader);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorDamper::Impl::m_BuildGeometry()
{
  m_geomdata.m_bufPoints.count = 0;
  m_geomdata.m_bufColors.count = 0;
  m_parts.count      = 0;
  m_bufIColors.count = 0;

  size_t k = 0;
  for (const auto& body : vbodies) {
    if (!b2Body_IsValid(body)) {
      m_parts.setItemAt(k, {0, 0});
      ++m_parts.count;
      m_bufIColors.setItemAt(k, {1,1,1,1});
      ++m_bufIColors.count;
      ++k; continue;
    }
    static constexpr i32 MAX_SHAPES = 4;
    b2ShapeId shps[MAX_SHAPES];
    auto isc = b2Body_GetShapeCount(body);
    b2Body_GetShapes(body, shps, isc < MAX_SHAPES ? isc : MAX_SHAPES);

    auto mat = b2Shape_GetSurfaceMaterial(shps[0]);
    Color co = RC(mat.customColor);
    m_bufIColors.setItemAt(k, ColorNormalize(co));
    ++m_bufIColors.count;

    i32 geom_start = i32(m_geomdata.m_bufPoints.count);
    for (auto i = 0; i < isc && i < MAX_SHAPES; ++i) {
      auto type = b2Shape_GetType(shps[i]);
      if (type == b2_capsuleShape) {
        auto caps = b2Shape_GetCapsule(shps[i]);
        m_AddCapsuleGeom(caps.center1, caps.center2, caps.radius);
      } else if (type == b2_polygonShape) {
        auto poly = b2Shape_GetPolygon(shps[i]);
        m_AddPolygonGeom(poly);
      }
    }
    i32 geom_end = i32(m_geomdata.m_bufPoints.count);
    m_parts.setItemAt(k, {geom_start, geom_end - geom_start});
    ++m_parts.count;
    ++k;
  }

  m_shader = LoadShader(
      shaderpath("actor_damper_330.vs").c_str()
    , shaderpath("actor_damper_330.fs").c_str());
  m_shader.locs[SHADER_LOC_VERTEX_POSITION] =
    GetShaderLocationAttrib(m_shader, "vertexPosition");
  m_shader.locs[SHADER_LOC_VERTEX_COLOR] =
    GetShaderLocationAttrib(m_shader, "vertexColor");
  m_iLocMVP           = GetShaderLocation(m_shader, "mvp");
  m_iLocPartTransform = GetShaderLocation(m_shader, "partTransform");
  m_iLocPartColor     = GetShaderLocation(m_shader, "partColor");

  i32 ipos = m_shader.locs[SHADER_LOC_VERTEX_POSITION];
  i32 ivco = m_shader.locs[SHADER_LOC_VERTEX_COLOR];

  m_uVBO = rlLoadVertexBuffer(&m_geomdata, sizeof(m_geomdata), false);

  m_uVAO = rlLoadVertexArray();
  rlEnableVertexArray(m_uVAO);
  rlEnableVertexBuffer(m_uVBO);
  rlSetVertexAttribute(ipos, 3, RL_FLOAT, false, 0, 0);
  rlEnableVertexAttribute(ipos);
  rlSetVertexAttribute(ivco, 4, RL_FLOAT, false, 0
    , i32(offsetof(decltype(m_geomdata), m_bufColors)));
  rlEnableVertexAttribute(ivco);
  rlDisableVertexBuffer();
  rlDisableVertexArray();

  std::printf("[DAMPER]: m_BuildGeometry(): %zu parts, %zu verts, shader=%u VAO=%u\n"
    , m_parts.count, m_geomdata.m_bufPoints.count
    , m_shader.id, m_uVAO);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD — geometria capsula in spazio locale (identico ad ActorPuppet)
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorDamper::Impl::m_AddCapsuleGeom(
  const b2Vec2& c1, const b2Vec2& c2, f32 r)
{
  auto& cnv = Conv::GetInstance();

  const f32 sx1 = cnv.x_w2s(c1.x), sy1 = cnv.y_w2s(c1.y);
  const f32 sx2 = cnv.x_w2s(c2.x), sy2 = cnv.y_w2s(c2.y);
  const f32 sr  = cnv.x_w2s(r);

  f32 dx = sx2 - sx1, dy = sy2 - sy1;
  f32 len = sqrtf(dx*dx + dy*dy);
  f32 ux  = (len > 0.0f) ? dx/len : 0.0f;
  f32 uy  = (len > 0.0f) ? dy/len : 1.0f;
  f32 px  = -uy * sr;
  f32 py  =  ux * sr;

  auto& pts = m_geomdata.m_bufPoints;
  auto& col = m_geomdata.m_bufColors;
  const Vector4 w = { 1.0f, 1.0f, 1.0f, 1.0f };

  Vector3 A = { sx1+px, sy1+py, 0.0f };
  Vector3 B = { sx1-px, sy1-py, 0.0f };
  Vector3 C = { sx2-px, sy2-py, 0.0f };
  Vector3 D = { sx2+px, sy2+py, 0.0f };
  {
    size_t b = pts.count;
    pts.data[b+0]=A; pts.data[b+1]=C; pts.data[b+2]=B; // CCW
    pts.data[b+3]=A; pts.data[b+4]=D; pts.data[b+5]=C; // CCW
    for (int i = 0; i < 6; ++i) { col.data[b+i] = w; }
    pts.count += 6; col.count += 6;
  }

  const f32 axis_angle = atan2f(uy, ux);

  // semicerchio lato c2
  {
    size_t b = pts.count;
    for (size_t i = 0; i < SEGS/2; ++i) {
      f32 a0 = axis_angle - f32(PI)/2.0f + f32(i  ) * f32(PI) / f32(SEGS/2);
      f32 a1 = axis_angle - f32(PI)/2.0f + f32(i+1) * f32(PI) / f32(SEGS/2);
      pts.data[b+0] = { sx2, sy2, 0.0f };
      pts.data[b+1] = { sx2 + sr*cosf(a1), sy2 + sr*sinf(a1), 0.0f }; // CCW
      pts.data[b+2] = { sx2 + sr*cosf(a0), sy2 + sr*sinf(a0), 0.0f };
      col.data[b+0] = col.data[b+1] = col.data[b+2] = w;
      b += 3;
    }
    pts.count = b; col.count = b;
  }

  // semicerchio lato c1
  {
    size_t b = pts.count;
    for (size_t i = 0; i < SEGS/2; ++i) {
      f32 a0 = axis_angle + f32(PI)/2.0f + f32(i  ) * f32(PI) / f32(SEGS/2);
      f32 a1 = axis_angle + f32(PI)/2.0f + f32(i+1) * f32(PI) / f32(SEGS/2);
      pts.data[b+0] = { sx1, sy1, 0.0f };
      pts.data[b+1] = { sx1 + sr*cosf(a1), sy1 + sr*sinf(a1), 0.0f }; // CCW
      pts.data[b+2] = { sx1 + sr*cosf(a0), sy1 + sr*sinf(a0), 0.0f };
      col.data[b+0] = col.data[b+1] = col.data[b+2] = w;
      b += 3;
    }
    pts.count = b; col.count = b;
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD — triangle fan da poligono convesso (identico ad ActorPuppet)
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorDamper::Impl::m_AddPolygonGeom(const b2Polygon& poly)
{
  auto& cnv = Conv::GetInstance();
  auto& pts = m_geomdata.m_bufPoints;
  auto& col = m_geomdata.m_bufColors;
  const Vector4 w = { 1.0f, 1.0f, 1.0f, 1.0f };

  for (i32 i = 1; i < poly.count - 1; ++i) {
    size_t b = pts.count;
    pts.data[b+0] = { cnv.x_w2s(poly.vertices[  0].x)
                    , cnv.y_w2s(poly.vertices[  0].y), 0.0f };
    pts.data[b+1] = { cnv.x_w2s(poly.vertices[i+1].x) // CCW
                    , cnv.y_w2s(poly.vertices[i+1].y), 0.0f };
    pts.data[b+2] = { cnv.x_w2s(poly.vertices[  i].x)
                    , cnv.y_w2s(poly.vertices[  i].y), 0.0f };
    col.data[b+0] = col.data[b+1] = col.data[b+2] = w;
    pts.count += 3; col.count += 3;
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2BodyId stemcapsulax::ActorDamper::Impl::m_CreateBox(b2WorldId wid
  , f32 x, f32 y, const b2Vec2& size, f32 density, Color c)
{
  b2BodyDef bd = b2DefaultBodyDef();
  bd.type = b2_dynamicBody;
  bd.angularDamping = 0.6f;
  bd.position = b2Vec2{ x, y };
  bd.rotation = b2MakeRot(0.0f * (B2_PI / 180.0f));
  b2BodyId bodyId = b2CreateBody(wid, &bd);
  b2Polygon box = b2MakeBox(size.x / 2.0f, size.y / 2.0f);
  b2ShapeDef sd = b2DefaultShapeDef();
  sd.enableContactEvents = true;
  sd.enableHitEvents = true;
  sd.density = density;
  sd.material.restitution = 0.8f;
  sd.material.friction = 0.2f;
  sd.material.customColor =
      u32(c.a) << 24
    | u32(c.r) << 16
    | u32(c.g) <<  8
    | u32(c.b);
  b2CreatePolygonShape(bodyId, &sd, &box);
  return bodyId;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2BodyId stemcapsulax::ActorDamper::Impl::m_CreateCapsule(b2WorldId wid
  , f32 x, f32 y, const b2Vec2& p0, const b2Vec2& p1, f32 radius
  , f32 density, Color c)
{
  b2BodyDef bd = b2DefaultBodyDef();
  bd.type = b2_dynamicBody;
  bd.angularDamping = 0.6f;
  bd.position = b2Vec2{ x, y };
  bd.rotation = b2MakeRot(0.0f * (B2_PI / 180.0f));
  b2BodyId bodyId = b2CreateBody(wid, &bd);
  b2Capsule capsule = { p0, p1, radius };
  b2ShapeDef sd = b2DefaultShapeDef();
  sd.enableContactEvents = true;
  sd.enableHitEvents = true;
  sd.density = density;
  sd.material.restitution = 0.8f;
  sd.material.friction = 0.2f;
  sd.material.customColor =
      u32(c.a) << 24
    | u32(c.r) << 16
    | u32(c.g) <<  8
    | u32(c.b);
  b2CreateCapsuleShape(bodyId, &sd, &capsule);
  return bodyId;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
b2JointId stemcapsulax::ActorDamper::Impl::m_CreateJoint(b2WorldId wid
  , f32 x, f32 y, b2BodyId b0, b2BodyId b1, b2Vec2 angles)
{
  b2Vec2 pivot = { x, y };
  b2RevoluteJointDef jd = b2DefaultRevoluteJointDef();
  jd.bodyIdA = b0;
  jd.bodyIdB = b1;
  jd.localAnchorA = b2Body_GetLocalPoint(b0, pivot);
  jd.localAnchorB = b2Body_GetLocalPoint(b1, pivot);
  jd.lowerAngle = angles.x * PI;
  jd.upperAngle = angles.y * PI;
  jd.enableLimit = true;
  jd.maxMotorTorque = 10.0f;
  jd.motorSpeed = 0.0f;
  jd.enableMotor = true;
  jd.enableSpring = true;
  jd.hertz = 100;
  b2JointId jid = b2CreateRevoluteJoint(wid, &jd);
  return jid;
}
