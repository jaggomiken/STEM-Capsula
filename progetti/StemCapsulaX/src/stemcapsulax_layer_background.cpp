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
#include "stemcapsulax_layer_background.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_system.h"
#include <future>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::LayerBackground::Impl {
public:
  Impl();
 ~Impl();

  std::string m_strImagePathReq;
  std::string m_strImagePath;
  Image m_image;
  Texture m_texture;
  
  Shader m_shader;
  i32 m_iLocSeconds;
  i32 m_iLocFreqX;
  i32 m_iLocFreqY;
  i32 m_iLocAmpX;
  i32 m_iLocAmpY;
  i32 m_iLocSpeedX;
  i32 m_iLocSpeedY;

  std::future<Image> m_futImageLoading;
  void m_LoadImage(const Image&);
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::LayerBackground::LayerBackground()
: m_pImpl{ nullptr }
{
  m_pImpl = new(std::nothrow) Impl{};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::LayerBackground::~LayerBackground()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Layer::TypeID stemcapsulax::LayerBackground::type() const
{
  return Layer::TypeID(LayerType::kBACKGROUND);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::LayerBackground::setImagePath(const std::string& fn)
{
  m_pImpl->m_strImagePathReq = fn;
  m_pImpl->m_futImageLoading = std::async(std::launch::async
    , [=]() -> Image 
  {
    auto image = LoadImage(fn.c_str());
    return image;
  });
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
std::string stemcapsulax::LayerBackground::imagePath() const
{
  return m_pImpl->m_strImagePath;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::LayerBackground::clear()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::LayerBackground::show()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::LayerBackground::hide()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::LayerBackground::update()
{
  if (m_pImpl->m_futImageLoading.valid()) {
    if (std::future_status::ready ==  
      m_pImpl->m_futImageLoading.wait_for(std::chrono::milliseconds(0)))
    {
      m_pImpl->m_LoadImage(m_pImpl->m_futImageLoading.get());
    }
  }

  auto& tr = runner();
  tr.enumerate([=, &tr](TaskRunner::Task& task) {
    if (task.update) { task.update(tr, task); }
  });
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::LayerBackground::draw(RenderTexture2D& rtex)
{
  auto& st = Status::GetInstance();
  auto& dt = st.data();
  f32 secs = dt.sysinf.fSecondsElapsed;
  SetShaderValue(m_pImpl->m_shader, m_pImpl->m_iLocSeconds
    , &secs, SHADER_UNIFORM_FLOAT);
  BeginShaderMode(m_pImpl->m_shader);    
  DrawTexture(m_pImpl->m_texture
    , (dt.sysinf.iWindowWidth  / 2) - (m_pImpl->m_texture.width  / 2)
    , (dt.sysinf.iWindowHeight / 2) - (m_pImpl->m_texture.height / 2)
    , WHITE);
  EndShaderMode();
  auto& tr = runner();
  tr.enumerate([=, &tr, &rtex](TaskRunner::Task& task) {
    if (task.draw) { task.draw(tr, task, rtex); }
  });
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::LayerBackground::Impl::Impl()
{
  // alcune di queste variabili andranno spostate nella classe
  m_shader = LoadShader(0, shaderpath("raylib_wave.fs").c_str());
  m_iLocSeconds = GetShaderLocation(m_shader, "seconds");
  m_iLocFreqX   = GetShaderLocation(m_shader, "freqX");
  m_iLocFreqY   = GetShaderLocation(m_shader, "freqY");
  m_iLocAmpX    = GetShaderLocation(m_shader, "ampX");
  m_iLocAmpY    = GetShaderLocation(m_shader, "ampY");
  m_iLocSpeedX  = GetShaderLocation(m_shader, "speedX");
  m_iLocSpeedY  = GetShaderLocation(m_shader, "speedY");

  f32 freqX  = 25.0f;
  f32 freqY  = 25.0f;
  f32 ampX   =  1.0f;
  f32 ampY   =  1.0f;
  f32 speedX =  2.0f;
  f32 speedY =  2.0f;

  f32 ss[] = { f32(GetScreenWidth()), f32(GetScreenHeight()) };
  SetShaderValue(m_shader, GetShaderLocation(m_shader, "size")
    , &ss, SHADER_UNIFORM_VEC2);
  SetShaderValue(m_shader,  m_iLocFreqX,  &freqX, SHADER_UNIFORM_FLOAT);
  SetShaderValue(m_shader,  m_iLocFreqY,  &freqY, SHADER_UNIFORM_FLOAT);
  SetShaderValue(m_shader,   m_iLocAmpX,   &ampX, SHADER_UNIFORM_FLOAT);
  SetShaderValue(m_shader,   m_iLocAmpY,   &ampY, SHADER_UNIFORM_FLOAT);
  SetShaderValue(m_shader, m_iLocSpeedX, &speedX, SHADER_UNIFORM_FLOAT);
  SetShaderValue(m_shader, m_iLocSpeedY, &speedY, SHADER_UNIFORM_FLOAT);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::LayerBackground::Impl::~Impl()
{
  UnloadShader(m_shader);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::LayerBackground::Impl::m_LoadImage(const Image& img)
{
  if (!m_strImagePath.empty()) { UnloadTexture(m_texture); }
  m_strImagePath = m_strImagePathReq;
  m_image        = img;
  m_texture      = LoadTextureFromImage(m_image);
  UnloadImage(m_image);
  m_strImagePathReq = {};
}
