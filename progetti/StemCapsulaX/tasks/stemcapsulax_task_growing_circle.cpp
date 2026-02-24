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
#include "stemcapsulax_task_growing_circle.h"
#include "stemcapsulax_director.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_audio_manager.h"
#include "stemcapsulax_task_runner.h"
#include "stemcapsulax_scene_manager.h"
#include "stemcapsulax_scene_layered.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STRUCT DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
struct Task_GrowingCircle {
   f32 m_fCircleRadius;
   f32 m_fRadiusScale;
  bool m_bDrawCircle;
  bool m_bIncreaseRadius;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::TaskRunner::Task 
  stemcapsulax::CreateTask_GrowingCircle(const std::function<void(f32)>& cb)
{
  static Task_GrowingCircle taskcrosshair;
  taskcrosshair.m_fCircleRadius =  1.0f;
  taskcrosshair.m_fRadiusScale  = 50.0f;
  TaskRunner::Task task;
  task.name      = "00_GrowingCircle";
  task.pUserData = &taskcrosshair;
  task.update    = [cb](TaskRunner& r, TaskRunner::Task& t) {
    Task_GrowingCircle* pT = reinterpret_cast<Task_GrowingCircle*>(t.pUserData);
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
      pT->m_bDrawCircle = true;
      if (pT->m_bIncreaseRadius) {
        pT->m_fCircleRadius += pT->m_fRadiusScale * (1.0f / GetFPS());
      }
    } else {
      pT->m_fCircleRadius   = .0f;
      pT->m_bDrawCircle     = false;
      pT->m_bIncreaseRadius = true;
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (IsKeyDown(KEY_LEFT_CONTROL)) {
        if (cb) { cb(pT->m_fCircleRadius); }
        pT->m_bIncreaseRadius = false;
      }
    }
  };
  task.draw = [](TaskRunner& r, TaskRunner::Task& t, RenderTexture2D& rtex) {
    auto mp = GetMousePosition();
    Task_GrowingCircle* pT = reinterpret_cast<Task_GrowingCircle*>(t.pUserData);
    if (pT->m_bDrawCircle) {
      f32 fr = pT->m_fCircleRadius;
      DrawCircle(mp.x, mp.y, fr, Fade(GRAY, .6f));
      DrawText(TextFormat("r = %.6f", pT->m_fCircleRadius)
        , mp.x, mp.y + 12, 10, WHITE);
    }
  };
  return task;
}
