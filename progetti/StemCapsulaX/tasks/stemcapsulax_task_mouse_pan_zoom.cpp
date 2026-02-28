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
#include "stemcapsulax_task_mouse_pan_zoom.h"
#include "stemcapsulax_director.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_audio_manager.h"
#include "stemcapsulax_task_runner.h"
#include "stemcapsulax_scene_manager.h"
#include "stemcapsulax_scene_layered.h"
#include "stemcapsulax_box2d_proxy.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::TaskRunner::Task 
  stemcapsulax::CreateTask_MousePanZoom(Camera2D& camera)
{
  TaskRunner::Task task;
  task.name      = "00_MousePan";
  task.pUserData = nullptr;
  task.draw      = {};
  task.update    = [&camera](TaskRunner& r, TaskRunner::Task& t) {
    camera.zoom = expf(logf(camera.zoom) + (f32(GetMouseWheelMove()) * 0.1f));
    if      (camera.zoom > 3.0f) { camera.zoom = 3.0f; }
    else if (camera.zoom < 0.1f) { camera.zoom = 0.1f; }
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
      auto dt = Vector2Scale(GetMouseDelta(), -1.0f / camera.zoom);
      camera.target = Vector2Add(camera.target, dt);
    }
    if (IsKeyPressed(KEY_R)) {
      auto& cnv = Conv::GetInstance();
      f32 cx =  cnv.fScreenWidth / 2.0f;
      f32 cy = cnv.fScreenHeight / 2.0f;
      camera.target   = { cx, cy };
      camera.offset   = { cx, cy };
      camera.zoom     = 1.0f;
      camera.rotation = 0.0f;
    }
  };
  return task;
}
