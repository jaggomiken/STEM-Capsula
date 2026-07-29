/*
  ==STRUTTURA DI UN OGGETTO DEL SISTEMA==

  { cx: 0, cy: 0, x: 0, y: 0, r: 0
    , type: "GRID", name: "grid", enabled: 0
    ,   shadow: { color: {r: 0, g: 0, b: 0, a: 1 }, blur: 0 }
    ,   border: {r:   0, g:   0, b:   0, a: 1 }
    ,     fill: {r:   0, g:   0, b:   0, a: 1 }
    , drawinfo: {} // stato del drawing
    , motiinfo: {} // stato della motion
    , motparam: {
            move_enable: 0
        , fx: 0.0, fy: 0.0, fR: 0.0
        , Ax:   0, Ay:   0, Ar: 0
        , color_enable: 0
        , fh: 0.0, fs:   0, fl:   0
        , Ah: 0.0, As:   0, Al:   0
        }
    , fn_motion: null
    ,   fn_draw: draw_crossline
    , fn_gradmk: null
  }
*/

const vobjects = [
    object_make_grid()
  , { cx: 0, cy: 0, x: 0, y: 0, r: 60
      , type: "PLAYER", name: "player_1", enabled: 1
      ,   shadow: { color: {h: 0, s: 30, l: 40, a: 1 }, blur: 32 }
      ,   border: {h:   0, s: 100, l: 100, a: 1.0 }
      ,     fill: {h:   0, s:  60, l:  60, a: 0.6 }
      , drawinfo: {} // stato del drawing
      , motiinfo: {} // stato della motion
      , motparam: {
             move_enable: 0
          , fx: 0.0, fy: 0.0, fR: 0.0
          , Ax:   0, Ay:   0, Ar: 0
          , color_enable: 0
          , fh: 0.0, fs:   0, fl:   0
          , Ah: 0.0, As:   0, Al:   0
          }
      , fn_motion: motion_player
      ,   fn_draw: draw_player
      , fn_gradmk: null
      ,       image: null
      ,    imagesrc: "player_1.png"
      , imageloaded: 0
    }
];