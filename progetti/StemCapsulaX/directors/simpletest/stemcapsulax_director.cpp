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
#include "stemcapsulax_director.h"
#include "stemcapsulax_audio_manager.h"
#include "stemcapsulax_scene_layered.h"
#include "stemcapsulax_scene_manager.h"
#include "stemcapsulax_task_crosshair.h"
#include "stemcapsulax_task_growing_circle.h"
#include "stemcapsulax_task_energy_circle.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * Il Director decide quali scene creare (sempre come static) e registrare
 * nel gestore delle scene. Inoltre il director decide quali task inserire
 * nelle varie scene (i task sono come attori). In genere una scena a layer
 * può fare il subclassing di LayeredScane e quindi fornire i livelli in
 * proprio. Ma il director potrebbe usare direttamente una LayeredScene e
 * popolarla con propri layer dedicati. Ricordiamo sempre che i task sono
 * di scene e anche di livello.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Director::PrepareAll(int argc, char* argv[])
{
  static LayeredScene scene;
  auto& tr = scene.runner();
  tr.taskAdd(CreateTask_CrossHair());
  tr.taskAdd(CreateTask_GrowingCircle());
  tr.taskAdd(CreateTask_EnergyCircle());
  auto& sm = SceneManager::GetInstance();
  sm.addScene(&scene);
  sm.setCurrentSceneByIndex(0);
  sm.currentScene().show();
}
