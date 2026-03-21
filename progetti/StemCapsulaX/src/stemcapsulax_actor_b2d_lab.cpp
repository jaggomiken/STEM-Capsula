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
#include "stemcapsulax_actor_b2d_lab.h"
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
class stemcapsulax::ActorB2DLab::Impl {
public:
  Impl(b2WorldId wid, const b2Vec2& center, f32 scale);
 ~Impl();

  b2WorldId m_wid;
  b2Vec2 m_center;
  f32 m_fScale;
  std::vector<b2BodyId> vbodies;

  Shader m_shader;
  u32 m_uVAO;
  u32 m_uVBO;
  u32 m_uVBOTrs;
  static constexpr size_t N = 1 * 1024 * 1024; // numero massimo
  buffer< Matrix, N> m_bufTransfo;
  __attribute__((__packed__)) struct shaderdata_t {
    buffer<Vector3, 3> m_bufPoints;
    buffer<Vector4, 3> m_bufColors;
  } m_shaderdata;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorB2DLab::ActorB2DLab(b2WorldId wid, const b2Vec2& center
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
stemcapsulax::ActorB2DLab::~ActorB2DLab()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorB2DLab::update()
{
  
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::ActorB2DLab::draw(RenderTexture2D& rtex, Layer& layer)
{
  /* 
   * ALCUNE NOTE.
   * Se voglio utilizzare il rendering mediante RLGL, per ogni vertex buffer,
   * prima della chiamata di disegno della primitiva, bisogna sempre (sembra)
   * scrivere la terna: rlEnableVertexBuffer, rlSetVertexAttribute e
   * rlEnableVertexAttribute. Ovviamente bisogna aver abilitato il VAO con
   * rlEnableVertexArray. Dopo averne fatto uso, bisogna sempre disabilitare
   * quanto impiegato. Questo si fa mediante: rlDisableVertexBuffer e poi
   * rlDisableVertexArray.
   */
  auto& camera = static_cast<LayerBox2D&>(layer).camera();
  auto p0 = GetScreenToWorld2D({ .0f, .0f }, camera);
  auto p1 = GetScreenToWorld2D({ 
      f32(rtex.texture.width)
    , f32(rtex.texture.height) }, camera);
  BeginShaderMode(m_pImpl->m_shader);
  auto& cnv = Conv::GetInstance();
  const f32 r = cnv.x_w2s(15.0f);
  auto coo = b2Vec2{ 
      cnv.x_w2s(m_pImpl->m_center.x)
    , cnv.y_w2s(m_pImpl->m_center.y) };

#if 0
  { // questo funziona
    auto p0 = m_pImpl->m_vPoints.at(0);
    auto p1 = m_pImpl->m_vPoints.at(1);
    auto p2 = m_pImpl->m_vPoints.at(2);
    DrawTriangle({ p0.x, p0.y }, { p1.x, p1.y }, { p2.x, p2.y }, RED);
  }
#endif  

#if 0
  { // questo funziona
    auto p0 = m_pImpl->shaderdata.m_bufPoints.itemAt(0);
    auto p1 = m_pImpl->shaderdata.m_bufPoints.itemAt(1);
    auto p2 = m_pImpl->shaderdata.m_bufPoints.itemAt(2);
    rlBegin(RL_TRIANGLES);
      rlVertex3f(p0.x, p0.y, p0.z);
      rlVertex3f(p1.x, p1.y, p1.z);
      rlVertex3f(p2.x, p2.y, p2.z);
    rlEnd();
  }
#endif

#if 1 // questo funziona (il bypass del batch di raylib richiede di impostare a mano tutto!)
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
  rlDrawVertexArrayInstanced(0, 3, 5); rlCheckErrors();
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
stemcapsulax::ActorB2DLab::Impl::Impl(b2WorldId wid
  , const b2Vec2& c_in, f32 s /* scala */)
  : m_wid    {    wid }
  , m_center {   c_in }
  , m_fScale {      s }
{
  m_shader = LoadShader(
      shaderpath("actor_b2d_lab_330.vs").c_str()
    , shaderpath("actor_b2d_lab_330.fs").c_str());

  // Popola il vettore dei punti e dei colori per la forma di base
  auto& cnv = Conv::GetInstance();
  auto c = Vector2{ cnv.x_w2s(m_center.x), cnv.y_w2s(m_center.y) };
  m_shaderdata.m_bufPoints.data[0] = { c.x        ,         c.y, .0f };
  m_shaderdata.m_bufPoints.data[1] = { c.x + 10.0f,         c.y, .0f };
  m_shaderdata.m_bufPoints.data[2] = { c.x + 10.0f, c.y - 10.0f, .0f };
  m_shaderdata.m_bufColors.data[0] = { 1.0f, 1.0f, 1.0f, 1.0f };
  m_shaderdata.m_bufColors.data[1] = { 1.0f,  .0f, 1.0f, 1.0f };
  m_shaderdata.m_bufColors.data[2] = { 1.0f, 1.0f,  .0f, 1.0f };

  // Popola il vettore delle matrici
  const size_t szNumMat = 150;
  m_bufTransfo.count = szNumMat;
  for (size_t k = 0;k < szNumMat;++k) {
    m_bufTransfo.setItemAt(k
      , MatrixTranspose(MatrixTranslate(f32(k) * 15.0f, 0.0f, 0.0f)));
  }

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
  m_uVBOTrs = rlLoadVertexBuffer(m_bufTransfo.data, m_bufTransfo.countInBytes(), true);
  rlEnableVertexArray(m_uVAO);
  rlEnableVertexBuffer(m_uVBO);
  rlSetVertexAttribute(m_shader.locs[   SHADER_LOC_VERTEX_POSITION], 3, RL_FLOAT
    , false, 0, 0);
  rlSetVertexAttribute(m_shader.locs[      SHADER_LOC_VERTEX_COLOR], 4, RL_FLOAT
    , false, 0, offsetof(decltype(m_shaderdata), m_bufColors));
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
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::ActorB2DLab::Impl::~Impl()
{
  for (auto& b : vbodies) { b2DestroyBody(b); }
  vbodies.clear();
  UnloadShader(m_shader);
}
