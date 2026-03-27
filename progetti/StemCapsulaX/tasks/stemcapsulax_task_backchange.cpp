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
#include "stemcapsulax_task_backchange.h"
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
  stemcapsulax::CreateTask_BackChange(LayerBackground* pL
    , const std::string& imgpath
    , const std::string& fnprefix
    , i32& iImgNum_inout)
{
  TaskRunner::Task task;
  task.name      = "00_BackChange";
  task.pUserData = nullptr;
  task.draw      = {};
  task.update    = [pL, &iImgNum_inout, imgpath, fnprefix]
    (TaskRunner& r, TaskRunner::Task& t) {
    bool bchange = false;
    if (IsKeyPressed(KEY_LEFT)) {
      iImgNum_inout--; bchange = true;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
      iImgNum_inout++; bchange = true;
    }
    if (bchange) {
      if (iImgNum_inout < 0) { iImgNum_inout = 0; }
      static thread_local char tmp[8192];
      std::snprintf(tmp, sizeof(tmp), "%s/%s_%02u.png"
      , imgpath.c_str(), fnprefix.c_str(), iImgNum_inout);
      pL->setImagePath(imagepath(tmp));
    }
  };
  return task;
}
