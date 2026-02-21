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
#include "stemcapsulax_gui.h"
#include "stemcapsulax_scene_manager.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_audio_manager.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::GUI::Impl {
public:
  Impl();
 ~Impl();
  void draw();

  TextEditor m_teVrtxShader;
  TextEditor m_teFragShader;
  bool m_bShowVSEditor;
  bool m_bShowFSEditor;
  i32 m_iNumSceneTrig;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::GUI::GUI()
  : m_pImpl{ nullptr }
{
  m_pImpl = new(std::nothrow) Impl{};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Errore allocazione");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::GUI::GUI(GUI&& oth) noexcept
  : m_pImpl{ nullptr }
{
  m_pImpl = oth.m_pImpl;
  oth.m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::GUI& stemcapsulax::GUI::operator=(GUI&& oth) noexcept
{
  delete m_pImpl;
  m_pImpl = oth.m_pImpl;
  oth.m_pImpl = nullptr;
  return *this;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::GUI::~GUI()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::GUI::draw(const SystemInfo&)
{
  m_pImpl->draw();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::GUI::Impl::Impl()
: m_bShowVSEditor{ false }
, m_bShowFSEditor{ false }
, m_iNumSceneTrig{     0 }
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::GUI::Impl::~Impl()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::GUI::Impl::draw()
{
  auto& am = AudioManager::GetInstance();
  auto& sm = SceneManager::GetInstance();
  auto& st = Status::GetInstance();
  auto& dt = st.data();

  if (ImGui::Begin("STEM Capsula X - Control Center")) {
    ImGui::Separator();
    ImGui::TextUnformatted("System");
    ImGui::Separator();
    ImGui::Checkbox("Simulation Paused", &dt.bSimulationPaused);
    ImGui::Checkbox("HUD Visible", &dt.bShowHUD);
    ImGui::Separator();
    ImGui::Text("Current Scene (Type=%u)", sm.currentScene().type());
    ImGui::Separator();
    ImGui::Checkbox("Debug Draw Enabled", &dt.bDrawDebugEnabled);
    ImGui::PushItemWidth(120.0f);
    ImGui::InputInt("##NumSceneTrig", &m_iNumSceneTrig,1,10);
    ImGui::SameLine();
    if (ImGui::Button("TRIGGER")) {
      if (sm.hasCurrentScene()) {
        sm.currentScene().trigger(m_iNumSceneTrig);
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("CLEAR")) {
      if (sm.hasCurrentScene()) {
        sm.currentScene().clear();
      }
    }
    ImGui::SameLine();
    if (ImGui::Button(m_bShowVSEditor
      ? "Hide VSE" : "Show VSE")) {
      m_bShowVSEditor = !m_bShowVSEditor;
    }
    ImGui::SameLine();
    if (ImGui::Button(m_bShowFSEditor
      ? "Hide FSE" : "Show FSE")) {
      m_bShowFSEditor = !m_bShowFSEditor;
    }
    ImGui::Separator();
    ImGui::TextUnformatted("Audio Manager");
    ImGui::Separator();
    bool bAudioPlaying = am.isMainWavePlaying();
    if (ImGui::Checkbox("Main Wave playing", &bAudioPlaying)) {
      am.playMainWave(bAudioPlaying);
    }
    ImGui::Text("                          Total Samples: %-9u"
      , dt.sdAudioManager.uNTotalFrames);
    ImGui::Text("                 Samples Sent to Device: %-9u"
      , dt.sdAudioManager.uNStreamedSamples);
    ImGui::Text("          Frequency Analysis Queue Size: %-9u"
      , dt.sdAudioManager.uFTQueueSizeInSamples);
    ImGui::Text("Samples Dequeued for Frequency Analysis: %-9u"
      , dt.sdAudioManager.uNFTDequeuedSamples);
    ImGui::Text("Samples Processed by Frequency Analysis: %-9u"
      , dt.sdAudioManager.uNFTProcessedSamples);
  }
  ImGui::End();

  if (m_bShowVSEditor) {
    if (ImGui::Begin("Vertex Shader Editor")) {
      m_teVrtxShader.Render("Vertex Shader");
    }
    ImGui::End();
  }
  if (m_bShowFSEditor) {
    if (ImGui::Begin("Fragment Shader Editor")) {
      m_teFragShader.Render("Fragment Shader");
    }
    ImGui::End();
  }
}
