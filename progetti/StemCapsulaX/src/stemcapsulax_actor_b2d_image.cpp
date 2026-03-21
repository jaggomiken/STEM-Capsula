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
#include "stemcapsulax_box2d_proxy.h"
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

  static constexpr size_t SEGS  = 32;
  static constexpr size_t NVERTS = SEGS * 3;

  Shader m_shader;
  i32 m_iLocInstColor;
  u32 m_uVAO;
  u32 m_uVBO;
  u32 m_uVBOTrs; // trasformazione per istanza (buffer)
  u32 m_uVBOCol; // colore per istanza (buffer)
  static constexpr size_t N = 1 * 1024 * 1024; // numero massimo
  buffer< Matrix, N> m_bufTransfo; // trasformazione per istanza
  buffer<Vector4, N> m_bufIColors; // colore per istanza
  __attribute__((__packed__)) struct shaderdata_t {
    buffer<Vector3, NVERTS> m_bufPoints;
    buffer<Vector4, NVERTS> m_bufColors;
  } m_shaderdata;

  void m_GenerateCirclePoints(const b2Vec2& c, f32 radius);
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
    // aggiorna i colori nel vertex buffer relativo
    size_t k = 0;
    for (const auto& body : m_pImpl->vbodies) {
      b2ShapeId shp[1]; 
      auto isc = b2Body_GetShapeCount(body);
      b2Body_GetShapes(body, shp, 1);
      auto mat = b2Shape_GetSurfaceMaterial(shp[0]);
      Color co = RC(mat.customColor);
      m_pImpl->m_bufIColors.setItemAt(k++, ColorNormalize(co));
    }
    m_pImpl->m_bufIColors.count = k;
    rlUpdateVertexBuffer(m_pImpl->m_uVBOCol
      , m_pImpl->m_bufIColors.data
      , m_pImpl->m_bufIColors.countInBytes()
      , 0);
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
  auto& cnv = Conv::GetInstance();
  size_t k = 0;
  for (const auto& body : m_pImpl->vbodies) {
    if (b2Body_IsValid(body)) {
      b2ShapeId shp[1];
      auto pos = b2Body_GetPosition(body);
      auto isc = b2Body_GetShapeCount(body);
      b2Body_GetShapes(body, shp, 1);
      auto mat = b2Shape_GetSurfaceMaterial(shp[0]);
      Color co = RC(mat.customColor);
      m_pImpl->m_bufTransfo.setItemAt(k, MatrixTranspose(
        MatrixTranslate(cnv.x_w2s(pos.x), cnv.y_w2s(pos.y), 0.0f)));
    } else {
      m_pImpl->m_bufTransfo.setItemAt(k, MatrixIdentity());
    }
    ++k;
  }
  m_pImpl->m_bufTransfo.count = k;
  rlUpdateVertexBuffer(m_pImpl->m_uVBOTrs
    , m_pImpl->m_bufTransfo.data
    , m_pImpl->m_bufTransfo.countInBytes(), 0);

  Box2DProxy::RemoveBodiesOutsideRect(
    -200, -200, 500, 500, m_pImpl->vbodies);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorB2DImage::draw(RenderTexture2D& rtex, Layer& layer)
{
#if 0 // non usato per ora  
  auto& camera = static_cast<LayerBox2D&>(layer).camera();
  auto p0 = GetScreenToWorld2D({ .0f, .0f }, camera);
  auto p1 = GetScreenToWorld2D({ 
      f32(rtex.texture.width)
    , f32(rtex.texture.height) }, camera);
#endif 

  auto& cnv = Conv::GetInstance();
  BeginShaderMode(m_pImpl->m_shader);

#if 0 // questo funziona
  DrawCircle(
      cnv.x_w2s(m_pImpl->m_center.x)
    , cnv.y_w2s(m_pImpl->m_center.y)
    , 20.0f, WHITE);
#endif    

#if 0
  { // questo funziona
    for (size_t k = 0;k < m_pImpl->m_shaderdata.m_bufPoints.count;k+=3) {
      auto p0 = m_pImpl->m_shaderdata.m_bufPoints.itemAt(k+0);
      auto p1 = m_pImpl->m_shaderdata.m_bufPoints.itemAt(k+1);
      auto p2 = m_pImpl->m_shaderdata.m_bufPoints.itemAt(k+2);
      auto c0 = m_pImpl->m_shaderdata.m_bufColors.itemAt(k+0);
      auto c1 = m_pImpl->m_shaderdata.m_bufColors.itemAt(k+1);
      auto c2 = m_pImpl->m_shaderdata.m_bufColors.itemAt(k+2);

      DrawCircle(cnv.x_w2s(p0.x), cnv.y_w2s(p0.y), 10.0f, RED);
      DrawCircle(cnv.x_w2s(p1.x), cnv.y_w2s(p1.y), 10.0f, GREEN);
      DrawCircle(cnv.x_w2s(p2.x), cnv.y_w2s(p2.y), 10.0f, BLUE);

      rlBegin(RL_TRIANGLES);
         rlColor3f(cnv.x_w2s(c0.x), cnv.y_w2s(c0.y), c0.z);
        rlVertex3f(cnv.x_w2s(p0.x), cnv.y_w2s(p0.y), p0.z);
         rlColor3f(cnv.x_w2s(c1.x), cnv.y_w2s(c1.y), c1.z);
        rlVertex3f(cnv.x_w2s(p1.x), cnv.y_w2s(p1.y), p1.z);
         rlColor3f(cnv.x_w2s(c2.x), cnv.y_w2s(c2.y), c2.z);
        rlVertex3f(cnv.x_w2s(p2.x), cnv.y_w2s(p2.y), p2.z);
      rlEnd();
    }
  }
#endif

#if 1 // questo non funziona ancora bene
  Matrix matProjection = rlGetMatrixProjection();
  Matrix matModelView  = rlGetMatrixModelview();
  Matrix mvp = MatrixMultiply(matModelView, matProjection);
  int mvpLoc = GetShaderLocation(m_pImpl->m_shader, "mvp");
  SetShaderValueMatrix(m_pImpl->m_shader, mvpLoc, mvp);
  f32 white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
  SetShaderValue(m_pImpl->m_shader
    , m_pImpl->m_shader.locs[SHADER_LOC_COLOR_DIFFUSE]
    , white, SHADER_UNIFORM_VEC4);  
  rlEnableVertexArray(m_pImpl->m_uVAO);
  rlEnableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_POSITION]);
  rlEnableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_COLOR]);
  rlEnableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX]+0);
  rlEnableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX]+1);
  rlEnableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX]+2);
  rlEnableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX]+3);
  rlEnableVertexAttribute(m_pImpl->m_iLocInstColor);
  rlDrawVertexArrayInstanced(0, Impl::NVERTS, m_pImpl->vbodies.size()); rlCheckErrors();
  rlDisableVertexAttribute(m_pImpl->m_iLocInstColor);
  rlDisableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_POSITION]);
  rlDisableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_COLOR]);
  rlDisableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX]+0);
  rlDisableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX]+1);
  rlDisableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX]+2);
  rlDisableVertexAttribute(m_pImpl->m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX]+3);
  rlDisableVertexArray();
#endif

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

  // Popola il vettore dei punti e dei colori per la forma di base
  auto& cnv = Conv::GetInstance();
  m_GenerateCirclePoints(m_center, s*cnv.x_w2s(.5f));

  // Preleva l'indirizzo dell'attributo delle trasformazioni
  m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX] = 
    GetShaderLocationAttrib(m_shader, "instancedtransforms");
  m_shader.locs[   SHADER_LOC_VERTEX_POSITION] = 
    GetShaderLocationAttrib(m_shader, "vertexPosition");
  m_shader.locs[      SHADER_LOC_VERTEX_COLOR] = 
    GetShaderLocationAttrib(m_shader, "vertexColor");

  // Crea il vertex array object
  m_uVAO    = rlLoadVertexArray();
  m_uVBO    = rlLoadVertexBuffer(&m_shaderdata, sizeof(m_shaderdata), true);
  m_uVBOTrs = rlLoadVertexBuffer(m_bufTransfo.data, m_bufTransfo.sizeInBytes(), true);
  m_uVBOCol = rlLoadVertexBuffer(m_bufIColors.data, m_bufIColors.sizeInBytes(), true);
  rlEnableVertexArray(m_uVAO);
  rlEnableVertexBuffer(m_uVBO);
  rlSetVertexAttribute(m_shader.locs[   SHADER_LOC_VERTEX_POSITION], 3, RL_FLOAT
    , false, 0, 0);
  rlSetVertexAttribute(m_shader.locs[      SHADER_LOC_VERTEX_COLOR], 4, RL_FLOAT
    , false, 0, offsetof(decltype(m_shaderdata), m_bufColors));
  rlDisableVertexBuffer();
  rlEnableVertexBuffer(m_uVBOCol);    
  m_iLocInstColor = GetShaderLocationAttrib(m_shader, "instancedcolor");
  rlSetVertexAttribute(m_iLocInstColor, 4, RL_FLOAT, false, 0, 0);
  rlSetVertexAttributeDivisor(m_iLocInstColor, 1);
  rlDisableVertexBuffer();
  rlEnableVertexBuffer(m_uVBOTrs);    
  i32 ioff = 0;
  i32 iloc = m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX];
  rlSetVertexAttribute(iloc+0, 4, RL_FLOAT
    , false, sizeof(Matrix), ioff+(0*sizeof(Vector4)));
  rlSetVertexAttributeDivisor(iloc+0, 1);    
  rlSetVertexAttribute(iloc+1, 4, RL_FLOAT
    , false, sizeof(Matrix), ioff+(1*sizeof(Vector4)));
  rlSetVertexAttributeDivisor(iloc+1, 1);
  rlSetVertexAttribute(iloc+2, 4, RL_FLOAT
    , false, sizeof(Matrix), ioff+(2*sizeof(Vector4)));
  rlSetVertexAttributeDivisor(iloc+2, 1);
  rlSetVertexAttribute(iloc+3, 4, RL_FLOAT
    , false, sizeof(Matrix), ioff+(3*sizeof(Vector4)));
  rlSetVertexAttributeDivisor(iloc+3, 1);
  rlDisableVertexBuffer();
  rlDisableVertexArray();

  // Verifichiamo che sia tutto in ordine (-1 indica un errore [nello shader])
  std::printf("[IMPL] shader.id=%u VAO=%u VBO=%u VBOTrs=%u\n",
    m_shader.id, m_uVAO, m_uVBO, m_uVBOTrs);
  std::printf("[IMPL] locs: pos=%d col=%d inst=%d diffuse=%d mvp=%d\n",
    m_shader.locs[SHADER_LOC_VERTEX_POSITION],
    m_shader.locs[SHADER_LOC_VERTEX_COLOR],
    m_shader.locs[SHADER_LOC_VERTEX_INSTANCE_TX],
    m_shader.locs[SHADER_LOC_COLOR_DIFFUSE],
    GetShaderLocation(m_shader, "mvp"));  
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorB2DImage::Impl::~Impl()
{
  for (auto& b : vbodies) { if (b2Body_IsValid(b)) { b2DestroyBody(b); }}
  vbodies.clear();
  rlUnloadVertexBuffer(m_uVBOCol);
  rlUnloadVertexBuffer(m_uVBOTrs);
  rlUnloadVertexBuffer(m_uVBO);
  rlUnloadVertexArray(m_uVAO);
  UnloadShader(m_shader);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorB2DImage::Impl::m_GenerateCirclePoints(
  const b2Vec2& c, f32 r)
{
  std::printf("[ACTORB2DIMAGE]: m_GenerateCirclePoints(): radius=%.2f\n", r);
  m_shaderdata.m_bufPoints.count = NVERTS;
  m_shaderdata.m_bufColors.count = NVERTS;
  for (size_t i = 0; i < SEGS; ++i) {
    const f32 a0 = f32(2.0 * PI *  i      / SEGS);
    const f32 a1 = f32(2.0 * PI * (i + 1) / SEGS);
    const size_t b = i * 3;
    m_shaderdata.m_bufPoints.data[b + 0] = { c.x +            .0f, c.y +            .0f, .0f };
    m_shaderdata.m_bufPoints.data[b + 1] = { c.x + (r * cosf(a1)), c.y + (r * sinf(a1)), .0f };
    m_shaderdata.m_bufPoints.data[b + 2] = { c.x + (r * cosf(a0)), c.y + (r * sinf(a0)), .0f };
    m_shaderdata.m_bufColors.data[b + 0] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_shaderdata.m_bufColors.data[b + 1] = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_shaderdata.m_bufColors.data[b + 2] = { 1.0f, 1.0f, 1.0f, 1.0f };
  }
}
