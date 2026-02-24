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
#include "stemcapsulax_task_energy_circle.h"
#include "stemcapsulax_director.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_audio_manager.h"
#include "stemcapsulax_task_runner.h"
#include "stemcapsulax_scene_manager.h"
#include "stemcapsulax_scene_layered.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STRUCT DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
struct Task_EnergyCircle {
   f32 m_fExplosionEnergy;
  bool m_bDrawEnergyRange;
  bool m_bIncreaseEnergy;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::TaskRunner::Task 
  stemcapsulax::CreateTask_EnergyCircle(const std::function<void(f32)>& cb)
{
  static Task_EnergyCircle taskcrosshair;
  taskcrosshair.m_fExplosionEnergy =  1.0f;
  TaskRunner::Task task;
  task.name      = "00_EnergyCircle";
  task.pUserData = &taskcrosshair;
  task.update    = [cb](TaskRunner& r, TaskRunner::Task& t) {
    Task_EnergyCircle* pT = reinterpret_cast<Task_EnergyCircle*>(t.pUserData);
    if (IsKeyDown(KEY_LEFT_ALT)) {
      pT->m_bDrawEnergyRange = true;
      if (pT->m_bIncreaseEnergy) {
        pT->m_fExplosionEnergy += 80.0f * (1.0f / GetFPS());
      }
    } else {
      pT->m_fExplosionEnergy = .0f;
      pT->m_bDrawEnergyRange = false;
      pT->m_bIncreaseEnergy = true;
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (IsKeyDown(KEY_LEFT_ALT)) {
        if (cb) { cb(pT->m_fExplosionEnergy); }
        pT->m_bIncreaseEnergy = false;
      }
    }
  };
  task.draw = [](TaskRunner& r, TaskRunner::Task& t, RenderTexture2D& rtex) {
    auto mp = GetMousePosition();
    Task_EnergyCircle* pT = reinterpret_cast<Task_EnergyCircle*>(t.pUserData);
    if (pT->m_bDrawEnergyRange) {
      f32 fr = pT->m_fExplosionEnergy;
      DrawCircle(mp.x, mp.y, fr, Fade(RED, .6f));
      DrawText(TextFormat("E = %.6f", pT->m_fExplosionEnergy)
        , mp.x, mp.y + 12, 10, RED);
    }
  };
  return task;
}
