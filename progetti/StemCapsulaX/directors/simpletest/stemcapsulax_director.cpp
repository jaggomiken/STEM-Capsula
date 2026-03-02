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
 * STATIC FUNCTIONS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
static void RemoveBodiesOutsideRect(f32 x0, f32 y0, f32 x1, f32 y1
  , std::vector<b2BodyId>& vbodies);

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

  // crea gli attori puppets
  u32 np = 7;
  f32 fpuph = 84.0f, fseg = cnv.fWorldWidth / 7.0f, pupoff = 10.0f;
  const f32 ascales[] = { 1.0f, 1.1f, 1.2f, 1.3f, 1.2f, 1.1f, 1.0f };
  f32 fpupallw = np * (fseg + pupoff);
  static std::vector<ActorPuppet*> vpuppets;
  for (size_t k = 0; k < size_t(np);++k) {
    char name[64]; std::snprintf(name, sizeof(name), "Pup%zu", k);
    auto* pup = new(std::nothrow) ActorPuppet{lab2d.worldId()
      , { ((cnv.fWorldWidth + fpupallw) / 2.0f) - (k * (fseg + pupoff)) - 10.0f
        , cnv.fWorldHeight - fpuph }, ascales[k]
      , name, ActorPuppet::Options::kGROUNDBASEWWALLS };
    vpuppets.push_back(pup);
  }

  // crea gli attori dampers
  static std::vector<ActorDamper*> vdampers;
  u32 nd = 24;
  f32 fdamperw = 2.0f, off = 4.1f, fdampallw = nd * (fdamperw + off);
  for (size_t k = 0;k < nd;++k) {
    char name[64]; std::snprintf(name, sizeof(name), "Dam%zu", k);
    auto* pdam = new(std::nothrow) ActorDamper{lab2d.worldId()
    , { ((cnv.fWorldWidth + fdampallw) / 2.0f) - (k * (fdamperw + off)) - 3.0f
      , cnv.fWorldHeight - 5.0f }, 1.0f, 1.0f, name };
    vdampers.push_back(pdam);
  }

  // aggiungi gli attori ai layer
  for (auto* ppup : vpuppets) { lab2d.actorAdd(ppup); }
  for (auto* pdam : vdampers) { lab2d.actorAdd(pdam); }
  lab2d.camera().zoom = .6f;

  // funzione trigger esterna per la scena con vettore dei body
  static std::vector<b2BodyId> vbodies;
  auto fnExtTrg = [&cnv](u32 what) {
    char fn[128]; std::snprintf(fn, sizeof(fn), "%02u.png", what);
    stemcapsulax::Box2DBodyFromImage bfi;
    if (bfi.loadImage(stemcapsulax::imagepath(fn))) {
      if (!bfi.bodyCreate(lab2d.worldId()
        , cnv.fWorldWidth / 2.0f, -cnv.fWorldHeight / 2.0f, vbodies)) {
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

  // valore medio dell'energia totale da confrontare con il valore istantaneo
  static f32 fenergyaccum = .0f, fenergyavg = .0f;
  static f32 fnumcycles = .0f;

  // configura la callback nel gestore audio
  // questa callback viene chiamata nel main loop ad ogni update
  auto fnam = [&cnv, nd, np]( 
      f32 fperc
    , const std::vector<f32>& vleft
    , const std::vector<f32>& vrght
    , f32 fL_Energy
    , f32 fR_Energy
    , std::pair<f32,f32> pairFreqAmpMinLft
    , std::pair<f32,f32> pairFreqAmpMaxLft
    , std::pair<f32,f32> pairFreqAmpMinRgt
    , std::pair<f32,f32> pairFreqAmpMaxRgt) 
  {
    auto& st = stemcapsulax::Status::GetInstance();
    i32 fps = st.data().sysinf.iFPS;

    f32 fTot = fL_Energy + fR_Energy;
    f32 fDeltaTime = .0f;
    f32 fpspeedinc = 0.05f * std::cosf(fDeltaTime);

    fnumcycles   += 1.0f;
    fenergyaccum += fTot;
    fenergyavg = fenergyaccum / fnumcycles;

    size_t N = std::min<size_t>(vleft.size(),vrght.size()) / 2;
    for (size_t k = 0;k < N;++k) {
      size_t damperidx = k % nd;
      vdampers[damperidx]->behave(0, { vleft[k] + vrght[k] });
    }

    size_t szLK = 
      size_t((f32(N) * pairFreqAmpMaxLft.first) / (48000.0f / 2.0f));
    size_t szRK = 
      size_t((f32(N) * pairFreqAmpMaxRgt.first) / (48000.0f / 2.0f));

    size_t szPupLK = szLK % np;
    size_t szPupRK = szRK % np;

    // L'animazione delle piattaforma funziona calcolando il valor medio mobile
    // dell'energia totale e il segno della velocità verticale viene stabilito
    // dal fatto che l'energia istantanea sia superiore o inferiore alla media.
    // Il valore effettivo della velocità è invece stabilito dall'ampiezza del
    // coseno della frequenza massima per L e R (anche no).
    f32 fact = -20.0f * (fTot - fenergyavg);
    if (fperc <= 99.0f) {
      vpuppets.at(szPupLK)->moveRelative(.0f, fact + fpspeedinc);
      vpuppets.at(szPupRK)->moveRelative(.0f, fact + fpspeedinc);
    } else {
      // falli precipitare...
      for (auto* p : vpuppets) {
        p->moveRelative(.0f, +10.0f);
      }
    }

    // Gestisce lo zoom della camera sulla base dell'energia totale audio
    lab2d.camera().zoom = .49f + (fTot / 20.0f);

    // Gestisce l'automa a stati finiti per i movimenti dei puppet
    switch (coactx.cst) {
      case CoStatus::kUNDEFINED:
        coactx.cst = CoStatus::kWAITFOR;
        coactx.fWaitFor = 2.0f; 
        coactx.fWaitForStart = st.data().sysinf.fSecondsElapsed;
        coactx.cstNextAfterWaitFor = CoStatus::kOPENLEGS;
        break;
      case CoStatus::kOPENLEGS:
        // std::fprintf(stdout, "[AMCB]: OPENLEGS\n");
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
          // std::fprintf(stdout, "[AMCB]: LEFTARMUP\n");
          lab2d.enumerate([&](Actor* pA) {
            pA->behave(u64(ActorPuppet::Behaviour::kLEFTARMUP)
              , { pairFreqAmpMaxLft.second });
          });
        }
        if (pairFreqAmpMaxRgt.first >= 500.0f) {
          // std::fprintf(stdout, "[AMCB]: RIGHTARMUP\n");
          lab2d.enumerate([&](Actor* pA) {
            pA->behave(u64(ActorPuppet::Behaviour::kRIGHTARMUP)
              , { pairFreqAmpMaxRgt.second });
          });
        }
        break;
      default:
        break;
    }
    RemoveBodiesOutsideRect(-100, -200, 250, 250, vbodies);
#if 0
    std::fprintf(stdout
      , "[AMCB]: ZOOM=%f ST=%02u TOT=%f L=%.6f "
        "(MIN=[%f,%f],MAX=[%f,%f]) "
        "R=%.6f (MIN=[%f,%f],MAX=[%f,%f]) SOUNDPERC:%.2f%%\r"
      , lab2d.camera().zoom
      , u32(coactx.cst), fTot
      , fL_Energy
      , pairFreqAmpMinLft.first, pairFreqAmpMinLft.second
      , pairFreqAmpMaxLft.first, pairFreqAmpMaxLft.second
      , fR_Energy
      , pairFreqAmpMinRgt.first, pairFreqAmpMinRgt.second
      , pairFreqAmpMaxRgt.first, pairFreqAmpMaxRgt.second
      , fperc);
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
  // tr.taskAdd(CreateTask_CrossHair());
  tr.taskAdd(CreateTask_GrowingCircle(fnGrowingCircle));
  tr.taskAdd(CreateTask_EnergyCircle(fnExplosion));
  tr.taskAdd(CreateTask_MousePanZoom(lab2d.camera()));

  // aggiunge la scena al gestore
  auto& sm = SceneManager::GetInstance();
  sm.addScene(&scene);
  sm.setCurrentSceneByIndex(0);
  sm.currentScene().show();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC FUNCTIONS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
static void RemoveBodiesOutsideRect(f32 x0, f32 y0, f32 x1, f32 y1
  , std::vector<b2BodyId>& vbodies)
{
  std::vector<b2BodyId> vtoremove;
  for (size_t k = 0;k < vbodies.size();++k) {
    auto& b = vbodies.at(k);
    if (b2Body_IsValid(b)) {
      auto pos = b2Body_GetPosition(b);
      if ((pos.x >= x0) && (pos.x <= x1) && (pos.y >= y0) && (pos.y <= y1)) {
        // OK
      } else {
        vtoremove.push_back(b);
        vbodies[k] = b2_nullBodyId;
      }
    }
  }
  for (auto& b : vtoremove) { b2DestroyBody(b); }
  
#if 0
  if (!vtoremove.empty()) {
    std::fprintf(stdout
      , "[BOX2DPROXY]: Body destroyed %zu\n",vtoremove.size());
  }
#endif  
}