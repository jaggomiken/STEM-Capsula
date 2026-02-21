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
#include "stemcapsulax_task_crosshair.h"
#include "stemcapsulax_director.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_audio_manager.h"
#include "stemcapsulax_task_runner.h"
#include "stemcapsulax_scene_manager.h"
#include "stemcapsulax_scene_layered.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::TaskRunner::Task stemcapsulax::CreateTask_CrossHair()
{
  TaskRunner::Task task;
  task.name      = "00_CrossHair";
  task.pUserData = nullptr;
  task.update    = {};
  task.draw = [](TaskRunner& r, TaskRunner::Task& t, RenderTexture2D& rtex) {
    auto mp = GetMousePosition();
    DrawLine( 0, mp.y, rtex.texture.width, mp.y, Fade(GRAY, .8f));
    DrawLine(mp.x, 0, mp.x, rtex.texture.height, Fade(GRAY, .8f));
  };
  return task;
}
