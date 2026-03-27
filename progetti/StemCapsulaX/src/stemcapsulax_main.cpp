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
#include "stemcapsulax_system.h"
#include "stemcapsulax_scene_manager.h"
#include "stemcapsulax_gui.h"
#include "stemcapsulax_hud.h"
#include "stemcapsulax_status.h"
#include "stemcapsulax_scene_box2d.h"
#include "stemcapsulax_audio_manager.h"
#include "stemcapsulax_director.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * MACROS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define APP_VERSION                                                  "1.3.0"
#define APP_NAME                                              "STEMCAPSULAX"
#define APP_WAIT_FOR_SYNC                                                  1
#define APP_ENABLE_HIDPI                                                   1
#define APP_FULLSCREEN                                                     0
#define APP_W                                                           1080
#define APP_H                                                           1920
#define APP_FPS                                                           60
#define APP_DEBUGDRAW_ENABLE                                           false
#if defined(WIN32)
# undef APP_ENABLE_HIDPI
# define APP_ENABLE_HIDPI 0
#endif

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * ENTRY POINT
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
i32 main(i32 argc, char* argv[])
{
/* --------------------------------------------------------------------------
 * PREINIT (variabili locali e costanti eventuali)
 * -------------------------------------------------------------------------- */
  std::string strTitle {
    APP_NAME " " APP_VERSION " by prof. <Michele Iacobellis>"
  };
  std::string strMessg {
    "(press ESC to exit, F3 show/hide HUD, F5 show/hide GUI, 0-9 trigger, ENTER start)"
  };

  auto strfontpath = stemcapsulax::fontpath("NovaMono-Regular.ttf");
  const char* pfont = strfontpath.c_str();

/* --------------------------------------------------------------------------
 * INIT DI RAYLIB (detect del monitor, costruzione finestra e font)
 * -------------------------------------------------------------------------- */
  i32 ww = APP_W, wh = APP_H, fps = APP_FPS; // se fps altissimo, si abbasserà nel loop
  SetConfigFlags(FLAG_MSAA_4X_HINT
#if APP_WAIT_FOR_SYNC == 1  
    | FLAG_VSYNC_HINT
#endif
#if APP_ENABLE_HIDPI == 1
    | FLAG_WINDOW_HIGHDPI
#endif
  );
  InitWindow(ww, wh, strTitle.c_str());
  i32 cur_m = GetCurrentMonitor()
    , dis_w = GetMonitorWidth(cur_m)
    , dis_h = GetMonitorHeight(cur_m);
#if APP_FULLSCREEN == 1
  ToggleFullscreen();
#else
  SetWindowSize(ww > dis_w ? dis_w : ww, wh > dis_h ? dis_h : wh);
  std::printf("[" APP_NAME "]: Window size %dx%d, Monitor size %dx%d\n"
    , ww, wh, dis_w, dis_h);
#endif
  SetTargetFPS(fps);
  ww = GetScreenWidth();
  wh = GetScreenHeight();
  auto font = LoadFont(pfont);
  SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR); // nota bene!
  std::printf("[" APP_NAME "]: Display Width=%d Height=%d (FPS=%d).\n"
    , ww, wh, fps);

/* --------------------------------------------------------------------------
 * INIT DI IMGUI (configurazione dello style e font per ImGui)
 * -------------------------------------------------------------------------- */
  rlImGuiSetup(true /* Tema Dark */);
  ImGuiStyle& style = ImGui::GetStyle();
  style.FontSizeBase    = 24.0f;
  style.WindowRounding  =  3;
  style.TabRounding     =  3;
  style.PopupRounding   =  3;
  style.FrameRounding   =  3;
  style.ChildRounding   =  3;
  style.GrabRounding    =  3;
  style.FrameBorderSize =  1;
  style.TabRounding     =  1;
  style.TabBorderSize   =  1;
  ImGuiIO& io = ImGui::GetIO();
  auto fontimgui = io.Fonts->AddFontFromFileTTF(pfont);
  io.FontDefault = fontimgui;
  std::printf("[" APP_NAME "]: IMGUI Style & Fonts initialized.\n");

/* --------------------------------------------------------------------------
 * SYSTEMINFO (prepara le informazioni di sistema)
 * -------------------------------------------------------------------------- */
  stemcapsulax::SystemInfo sysinf;
  sysinf.bIsFullScreen   = (0 != APP_FULLSCREEN) ? true : false;
  sysinf.iWindowWidth    = ww;
  sysinf.iWindowHeight   = wh;
  sysinf.iFPS            = 0;
  sysinf.fSecondsElapsed = .0f;
  sysinf.fHiDpiScaleX    = GetWindowScaleDPI().x;
  sysinf.fHiDpiScaleY    = GetWindowScaleDPI().y;
  sysinf.strWindowTitle  = strTitle;
  sysinf.strTopMessage   = strMessg;
  sysinf.font            = font;

/* --------------------------------------------------------------------------
 * BOX2D CONFIG (configura il sistema di coordinate per Box2D)
 * -------------------------------------------------------------------------- */
  auto& cnv = stemcapsulax::Conv::GetInstance();
  cnv.fScreenWidth  = float(ww);
  cnv.fScreenHeight = float(wh);
  cnv.fWorldWidth   = 100.0f; // metri (da scegliere)
  cnv.fWorldHeight  = cnv.fWorldWidth * float(wh) / float(ww);
  std::printf("[" APP_NAME "]: Box2D, Screen %.1fx%.1f World %.1fx%.1f.\n"
    , cnv.fScreenWidth, cnv.fScreenHeight, cnv.fWorldWidth, cnv.fWorldHeight);
  std::printf("[" APP_NAME "]: Box2D, X 1m -> %.2f pix Y 1m -> %.2f pix.\n"
    , cnv.x_w2s(1.0f), cnv.y_w2s(1.0f));
  
/* --------------------------------------------------------------------------
 * INIT DEGLI OGGETTI DI BASE (creazione oggetti necessari)
 * -------------------------------------------------------------------------- */
  auto& aum = stemcapsulax::AudioManager::GetInstance();
  auto& sm  = stemcapsulax::SceneManager::GetInstance();
  auto& hud = stemcapsulax::HUD::GetInstance();
  static stemcapsulax::GUI gui;
  auto rtexForScene = LoadRenderTexture(ww, wh); // texture per la scena
  auto rtexForHUD   = LoadRenderTexture(ww, wh); // texture per lo HUD
  SetTextureFilter(rtexForScene.texture, TEXTURE_FILTER_BILINEAR);
  SetTextureFilter(rtexForHUD.texture, TEXTURE_FILTER_BILINEAR);

/* --------------------------------------------------------------------------
 * STATUS INIT (prepara lo stato globale del sistema)
 * -------------------------------------------------------------------------- */
  auto& sta = stemcapsulax::Status::GetInstance();
  sta.data().bSimulationPaused = false;
  sta.data().bDrawDebugEnabled = APP_DEBUGDRAW_ENABLE;
  sta.data().sysinf            = sysinf; // scrivi sysinf iniziale

/* --------------------------------------------------------------------------
 * DIRECTOR (prepara task, registra scene nello scene manager, ecc.)
 * -------------------------------------------------------------------------- */
  stemcapsulax::Director::PrepareAll(argc, argv);

/* --------------------------------------------------------------------------
 * AUDIO START (prepara l'audio principale, se disponibile)
 * -------------------------------------------------------------------------- */
  aum.loadMainWave((nullptr != argv[1]) ? argv[1] : "");

/* --------------------------------------------------------------------------
 * Variabili di controllo del ciclo principale (main loop o event loop)
 * -------------------------------------------------------------------------- */
  bool bExitLoop = false, bShowExitDialog = false;
  bool bShowHUD  = true, bShowGUI = true;

/* --------------------------------------------------------------------------
 * ACTIONKEYS (gestione dei tasti da 0 a 9 per il trigger)
 * -------------------------------------------------------------------------- */
  struct ActionKey { i32 iKey; u32 uSceneValue; };
  ActionKey aactions[] = {
      {  KEY_ZERO, 0 }, {  KEY_ONE, 1 }, { KEY_TWO, 2 }, { KEY_THREE, 3 }
    , {  KEY_FOUR, 4 }, { KEY_FIVE, 5 }, { KEY_SIX, 6 }, { KEY_SEVEN, 7 }
    , { KEY_EIGHT, 8 }, { KEY_NINE, 9 }
  };

/* --------------------------------------------------------------------------
 * MAINLOOP (gestione degli eventi e disegno della finestra)
 * -------------------------------------------------------------------------- */
  while (!bExitLoop) {
    /* ----------------------------------------------------------------------
     | GETINPUTS (prende tastiera, mouse, joypad, ecc...)
     * ---------------------------------------------------------------------- */
    if (IsKeyPressed(KEY_ESCAPE)) { bShowExitDialog = true; }
    if (IsKeyPressed(KEY_F3)) { bShowHUD = !bShowHUD; }
    if (IsKeyPressed(KEY_F5)) { bShowGUI = !bShowGUI; }
    if (IsKeyPressed(KEY_ENTER)) { aum.playMainWave(true); }
    if (IsKeyPressed(KEY_SPACE)) { 
      if (IsCursorHidden()) { ShowCursor(); } else { HideCursor(); }
    }
    for (size_t k = 0;k < STEMCAPSULAX_ARRAY_SIZE(aactions);++k) {
      if (IsKeyPressed(aactions[k].iKey)) {
        if (sm.hasCurrentScene()) {
          sm.currentScene().trigger(aactions[k].uSceneValue);
        }
      }
    }

    /* ----------------------------------------------------------------------
     | GETRAYLIBINFO (prende informazioni da RAYLIB e aggiorna stato)
     * ---------------------------------------------------------------------- */
    sysinf.iFPS = GetFPS(); // prende gli FPS effettivi
    sysinf.fSecondsElapsed += GetFrameTime();
    sta.data().sysinf.iFPS = sysinf.iFPS;
    sta.data().sysinf.fSecondsElapsed = sysinf.fSecondsElapsed;
    sta.data().bShowGUI = bShowGUI;
    sta.data().bShowHUD = bShowHUD;

    /* ----------------------------------------------------------------------
     | UPDATESTATUS (aggiorna lo stato degli oggetti dell'applicazione)
     * ---------------------------------------------------------------------- */
    if (sm.hasCurrentScene()) { sm.currentScene().update(); }
    hud.update(); // chiede allo HUD di aggiornare il prioprio stato interno
    aum.update(); // chiede a AudioManager di aggiornare lo stato interno
    sta.update(); // chiede a Status di aggiornarsi come lui sa
    
    /* ----------------------------------------------------------------------
     | DRAW (disegna gli oggetti nelle relative render texture, off screen)
     * ---------------------------------------------------------------------- */
    if (sm.hasCurrentScene()) { sm.currentScene().draw(rtexForScene); }
    hud.draw(rtexForHUD); // va sempre disegnata indipendentemente dal flag

    /* ----------------------------------------------------------------------
     | DRAWSCREEN (disegna gli oggetti sulla finestra: scena, hud e gui)
     * ---------------------------------------------------------------------- */
    BeginDrawing();
      ClearBackground(BLACK);
      float tw = float(rtexForScene.texture.width);
      float th = float(rtexForScene.texture.height);
      DrawTexturePro(rtexForScene.texture
        , { .0f, .0f, tw,-th }
        , { .0f, .0f, tw, th }
        , { .0f, .0f }, 0.0f, Fade(WHITE, 1.0f));
      tw = float(rtexForHUD.texture.width);
      th = float(rtexForHUD.texture.height);
      DrawTexturePro(rtexForHUD.texture
        , { .0f, .0f, tw,-th }
        , { .0f, .0f, tw, th }
        , { .0f, .0f }, 0.0f, Fade(WHITE, 1.0f));
    rlImGuiBegin();
      if (bShowGUI) { gui.draw(sysinf); }
      bExitLoop = stemcapsulax::HandleSystemExit(bShowExitDialog);
      bShowHUD  = sta.data().bShowHUD; // aggiorna a causa GUI
    rlImGuiEnd();
    EndDrawing();
  }

/* --------------------------------------------------------------------------
 * SHUTDOWN (chiusura di sotto-sistemi e finestra)
 * -------------------------------------------------------------------------- */
  aum.shutdown();
  rlImGuiShutdown();
  CloseWindow();
  return 0;
}
