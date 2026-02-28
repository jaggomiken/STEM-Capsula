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
#include "stemcapsulax_status.h"
#include "stemcapsulax_audio_manager.h"
#include "stemcapsulax_box2d_fromimage.h"
#include "stemcapsulax_box2d_proxy.h"
#include "stemcapsulax_scene_layered.h"
#include "stemcapsulax_scene_manager.h"
#include "stemcapsulax_layer_background.h"
#include "stemcapsulax_layer_3d.h"
#include "stemcapsulax_layer_box2d.h"
#include "stemcapsulax_task_crosshair.h"
#include "stemcapsulax_task_growing_circle.h"
#include "stemcapsulax_task_energy_circle.h"
#include "stemcapsulax_task_mouse_pan_zoom.h"
#include "stemcapsulax_box2d_proxy.h"
#include "stemcapsulax_actor_puppet.h"
#include "stemcapsulax_actor_damper.h"

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
    auto   mp = GetScreenToWorld2D(GetMousePosition(), lab2d.camera());
    auto& cnv = Conv::GetInstance();
    lab2d.circleAt(cnv.x_s2w(mp.x), cnv.y_s2w(mp.y), cnv.x_s2w(radius));
  };
  auto fnExplosion = [](f32 energy) {
    auto   mp = GetScreenToWorld2D(GetMousePosition(), lab2d.camera());
    auto& cnv = Conv::GetInstance();
    lab2d.explodeAt(cnv.x_s2w(mp.x), cnv.y_s2w(mp.y), energy);
  };

  auto& cnv = Conv::GetInstance();

  // crea gli attori
#if 1
  f32 fpuph = 46.0f;
  static ActorPuppet pup1{lab2d.worldId()
    , { cnv.fWorldWidth / 2.0f - 50.0f, cnv.fWorldHeight - fpuph }, 1.0f
    , "Pup1" };
  static ActorPuppet pup2{lab2d.worldId()
    , { cnv.fWorldWidth / 2.0f - 35.0f, cnv.fWorldHeight - fpuph }, 1.1f
    , "Pup2" };
  static ActorPuppet pup3{lab2d.worldId()
    , { cnv.fWorldWidth / 2.0f - 20.0f, cnv.fWorldHeight - fpuph }, 1.2f
    , "Pup3" };
  static ActorPuppet pup4{lab2d.worldId()
    , { cnv.fWorldWidth / 2.0f -  0.0f, cnv.fWorldHeight - fpuph }, 1.3f
    , "Pup4" };
  static ActorPuppet pup5{lab2d.worldId()
    , { cnv.fWorldWidth / 2.0f + 20.0f, cnv.fWorldHeight - fpuph }, 1.2f
    , "Pup5" };
  static ActorPuppet pup6{lab2d.worldId()
    , { cnv.fWorldWidth / 2.0f + 35.0f, cnv.fWorldHeight - fpuph }, 1.1f
    , "Pup6" };
  static ActorPuppet pup7{lab2d.worldId()
    , { cnv.fWorldWidth / 2.0f + 50.0f, cnv.fWorldHeight - fpuph }, 1.0f
    , "Pup7" };
#endif
  static std::vector<ActorDamper*> vdampers;
  u32 nd = 24;
  f32 fdamperw = 2.0f, off = 4.1f, fdampallw = nd * (fdamperw + off);
  for (size_t k = 0;k < nd;++k) {
    char name[64]; std::snprintf(name, sizeof(name), "Dam%zu", k);
    auto* pdam = new(std::nothrow) ActorDamper{lab2d.worldId()
    , { ((cnv.fWorldWidth + fdampallw) / 2.0f) - (k * (fdamperw + off)), cnv.fWorldHeight - 5.0f }, 1.0f, 1.0f
    , name };
    vdampers.push_back(pdam);
  }

  // aggiungi gli attori ai layer
#if 1
  lab2d.actorAdd(&pup1);
  lab2d.actorAdd(&pup2);
  lab2d.actorAdd(&pup3);
  lab2d.actorAdd(&pup4);
  lab2d.actorAdd(&pup5);
  lab2d.actorAdd(&pup6);
  lab2d.actorAdd(&pup7);
#endif  
  for (auto* pdam : vdampers) { lab2d.actorAdd(pdam); }
  lab2d.camera().zoom = .6f;

  // funzione trigger esternal per la scena
  static std::vector<b2BodyId> vbodies;
  auto fnExtTrg = [](u32 what) {
    char fn[128]; std::snprintf(fn, sizeof(fn), "%02u.png", what);
    stemcapsulax::Box2DBodyFromImage bfi;
    if (bfi.loadImage(stemcapsulax::imagepath(fn))) {
      if (!bfi.bodyCreate(lab2d.worldId(), .0f, .0f, vbodies)) {
        std::printf("[ERROR]: Cannot create bodies from image!\n");
      }
    }
  };
  scene.setExternalTriggerCallback(fnExtTrg);

  // automa a stati finiti per gestire la coreografia
  enum class CoStatus : u32 {
      kUNDEFINED
    , kLEFTARMUP
    , kRIGHTARMUP
    , kOPENLEGS
    , kCLOSELEGS
    , kMOVELEGS
    , kWAITFOR
  };

  static struct CoAutomataContext {
    CoStatus cst = CoStatus::kUNDEFINED;
    CoStatus cstNextAfterWaitFor = CoStatus::kUNDEFINED;
    f32 fWaitFor = .0f, fWaitForStart = .0f;
  } coactx;

  // configura la callback nel gestore audio
  // questa callback viene chiamata nel main loop ad ogni update
  auto fnam = [&cnv, nd]( 
      const std::vector<f32>& vleft
    , const std::vector<f32>& vrght
    , f32 fL_Energy
    , f32 fR_Energy
    , std::pair<f32,f32> pairFreqAmpMinLft
    , std::pair<f32,f32> pairFreqAmpMaxLft
    , std::pair<f32,f32> pairFreqAmpMinRgt
    , std::pair<f32,f32> pairFreqAmpMaxRgt) 
  {
#if 1
    auto& st = stemcapsulax::Status::GetInstance();
    i32 fps = st.data().sysinf.iFPS;

    f32 fTot = fL_Energy + fR_Energy;
    f32 fDeltaTime = .0f;

    size_t N = std::min<size_t>(vleft.size(),vrght.size()) / 2;
    for (size_t k = 0;k < N;++k) {
      size_t damperidx = k % nd;
      vdampers[damperidx]->behave(0, { vleft[k] + vrght[k] });
    }

    switch (coactx.cst) {
      case CoStatus::kUNDEFINED:
        coactx.cst = CoStatus::kWAITFOR;
        coactx.fWaitFor = 2.0f; 
        coactx.fWaitForStart = st.data().sysinf.fSecondsElapsed;
        coactx.cstNextAfterWaitFor = CoStatus::kOPENLEGS;
        break;
      case CoStatus::kOPENLEGS:
        std::fprintf(stdout, "[AMCB]: OPENLEGS\n");
        lab2d.enumerate([&](Actor* pA) {
          pA->behave(u64(ActorPuppet::Behaviour::kOPENLEGS), {});
        });
        coactx.cst = CoStatus::kWAITFOR;
        coactx.fWaitFor = .5f;
        coactx.fWaitForStart = st.data().sysinf.fSecondsElapsed;
        coactx.cstNextAfterWaitFor = CoStatus::kMOVELEGS;
        break;
      case CoStatus::kWAITFOR:
        fDeltaTime = st.data().sysinf.fSecondsElapsed - coactx.fWaitForStart;
        if (fDeltaTime >= coactx.fWaitFor) {
          coactx.cst = coactx.cstNextAfterWaitFor;
        } else {
          coactx.cst = CoStatus::kWAITFOR;
        }
        break;
      case CoStatus::kMOVELEGS:
        lab2d.enumerate([&](Actor* pA) {
          pA->behave(u64(ActorPuppet::Behaviour::kMOVELEGS)
            , { fTot, fTot });
        });
        if (pairFreqAmpMaxLft.first >= 500.0f) {
          std::fprintf(stdout, "[AMCB]: LEFTARMUP\n");
          lab2d.enumerate([&](Actor* pA) {
            pA->behave(u64(ActorPuppet::Behaviour::kLEFTARMUP)
              , { pairFreqAmpMaxLft.second });
          });
        }
        if (pairFreqAmpMaxRgt.first >= 500.0f) {
          std::fprintf(stdout, "[AMCB]: RIGHTARMUP\n");
          lab2d.enumerate([&](Actor* pA) {
            pA->behave(u64(ActorPuppet::Behaviour::kRIGHTARMUP)
              , { pairFreqAmpMaxRgt.second });
          });
        }
        break;
      default:
        break;
    }
    std::fprintf(stdout
      , "[AMCB]: ST=%02u TOT=%f L=%.6f "
        "(MIN=[%f,%f],MAX=[%f,%f]) "
        "R=%.6f (MIN=[%f,%f],MAX=[%f,%f])\r"
      , u32(coactx.cst), fTot
      , fL_Energy
      , pairFreqAmpMinLft.first, pairFreqAmpMinLft.second
      , pairFreqAmpMaxLft.first, pairFreqAmpMaxLft.second
      , fR_Energy
      , pairFreqAmpMinRgt.first, pairFreqAmpMinRgt.second
      , pairFreqAmpMaxRgt.first, pairFreqAmpMaxRgt.second);
    std::fflush(stdout);
#endif
  };
  AudioManager::GetInstance().registerDataCallback(fnam);

  // configura i layer di scena
  scene.layerAdd(&laback); // disegnato per primo
  scene.layerAdd(&la3d);   // disegnato per secondo
  scene.layerAdd(&lab2d);  // disegnato per ultimo (in primo piano)
  // configura i task di scena
  auto& tr = scene.runner();
  tr.taskAdd(CreateTask_CrossHair());
  tr.taskAdd(CreateTask_GrowingCircle(fnGrowingCircle));
  tr.taskAdd(CreateTask_EnergyCircle(fnExplosion));
  tr.taskAdd(CreateTask_MousePanZoom(lab2d.camera()));

  // aggiunge la scena al gestore
  auto& sm = SceneManager::GetInstance();
  sm.addScene(&scene);
  sm.setCurrentSceneByIndex(0);
  sm.currentScene().show();
}
