const pad  = 22;
const kFPS = 60.0, kT = 1000.0 / kFPS;
const kFD  = kT / 5000.0;
const kAcceptedKeys = [
    "ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight"
  , "Space"
];
let t = 0.0;
let bEnableClearRect     = true;
let bEnableGlow          = true;
let bEnableLighter       = false;
let bEnableKeyboardDebug = true;
let sCurrentKeys = new Set();
let AppStatus = {
  fps: 0.0, frametime: 0.0, t0: 0.0, nframes: 0
};