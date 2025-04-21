/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * LCG - LEZIONI COMPUTER GRAFICA
 * (C) 2025 Copyright by Michele Iacobellis
 * A project for students...
 * 
 * This file is part of LCG.
 *
 * LCG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCG. If not, see <http://www.gnu.org/licenses/>.
 * 
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * INCLUDE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#include "lcg.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * INCLUDE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DATI STATICI
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DEFINIZIONE STRUTTURA/CLASSE
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
struct ModulePrimo {
  ModulePrimo(): m_bVisible{false} {}
  bool operator() (lcg::ApplicationContext&);
  bool m_bVisible;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * DEFINIZIONE FUNZIONE/METODO
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool ModulePrimo::operator() (lcg::ApplicationContext& ac) 
{
  if (ImGui::Begin("INTRODUZIONE - Lezioni Computer Grafica")) {
    ImGui::TextWrapped(
      "Questa applicazione è un gestore centralizzato e "
      "modulare contenente funzioni per esplorare alcuni "
      "dei concetti principali della computer grafica.");
    ImGui::Separator();
    ImGui::TextWrapped("Realizzato da Michele Iacobellis");
    ImGui::Separator();
    const char* pTxtShow = "Mostra Demo ImGui";
    const char* pTxtHide = "Nascondi Demo ImGui";
    if (ImGui::Button(m_bVisible ? pTxtHide : pTxtShow)) {
      m_bVisible = !m_bVisible; 
    }
    if (m_bVisible) { 
      ImGui::ShowDemoWindow(&m_bVisible); 
    }
  }
  ImGui::End();
  return true;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * MODULE REGISTRATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
LCG_IMGUI_MODULE_REGISTER("Primo", ModulePrimo());