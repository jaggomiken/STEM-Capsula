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
#include "stemcapsulax_layer_background.h"
#include "stemcapsulax_layer_3d.h"
#include "stemcapsulax_layer_box2d.h"
#include "stemcapsulax_task_crosshair.h"
#include "stemcapsulax_task_growing_circle.h"
#include "stemcapsulax_task_energy_circle.h"
#include "stemcapsulax_box2d_proxy.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * Il Director decide quali scene creare (sempre come static) e registrare
 * nel gestore delle scene. Inoltre il director decide quali task inserire
 * nelle varie scene (i task sono come attori). In genere una scena a layer
 * può fare il subclassing di LayeredScene e quindi fornire i livelli in
 * proprio. Ma il director potrebbe usare direttamente una LayeredScene e
 * popolarla con propri layer dedicati. Ricordiamo sempre che i task sono
 * di scene e anche di livello.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Director::PrepareAll(int argc, char* argv[])
{
  /////////////////////////////////////////////////////////////////////////////
  // In questo esempio, il director configura esternamente i layer della scena
  // inserendo un background, un layer 3D e poi un layer Box2D. Inoltre
  // configura dei task che svolgono specifiche funzioni nella scena per
  // intero.
  /////////////////////////////////////////////////////////////////////////////
  static LayeredScene scene;
  static LayerBackground laback;
  static Layer3D la3d;
  static LayerBox2D lab2d;
  // inizializza i layer secondo le loro specificità
  laback.setImagePath(imagepath("backdemo.png")); // background texture
  auto fnGrowingCircle = [](f32 radius) {
    auto mp = GetScreenToWorld2D(GetMousePosition(), lab2d.camera());
    auto& cnv = Conv::GetInstance();
    lab2d.circleAt(cnv.x_s2w(mp.x), cnv.y_s2w(mp.y), cnv.x_s2w(radius));
  };
  auto fnExplosion = [](f32 energy) {
    auto mp = GetScreenToWorld2D(GetMousePosition(), lab2d.camera());
    auto& cnv = Conv::GetInstance();
    lab2d.explodeAt(cnv.x_s2w(mp.x), cnv.y_s2w(mp.y), energy);
  };

  // configura i layer di scena
  scene.layerAdd(&laback); // disegnato per primo
  scene.layerAdd(&la3d);   // disegnato per secondo
  scene.layerAdd(&lab2d);  // disegnato per ultimo (in primo piano)
  // configura i task di scena
  auto& tr = scene.runner();
  tr.taskAdd(CreateTask_CrossHair());
  tr.taskAdd(CreateTask_GrowingCircle(fnGrowingCircle));
  tr.taskAdd(CreateTask_EnergyCircle(fnExplosion));
  auto& sm = SceneManager::GetInstance();
  sm.addScene(&scene);
  sm.setCurrentSceneByIndex(0);
  sm.currentScene().show();
}
