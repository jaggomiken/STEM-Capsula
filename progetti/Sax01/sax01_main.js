function resize(e) {
  const cnv = document.getElementById("arena");
  const lgo = document.getElementById("logo_panel");  
  const mnb = document.getElementById("menubar");
  const ctx = cnv.getContext("2d");
  const 
      ww = window.innerWidth
    , wh = window.innerHeight
        - mnb.getBoundingClientRect().y
        - mnb.getBoundingClientRect().height
    , dp = window.devicePixelRatio;
  cnv.style[ "width"] = Math.floor(ww - pad) + "px";
  cnv.style["height"] = Math.floor(wh - pad) + "px";
  cnv.width  = Math.round(ww * dp);
  cnv.height = Math.round(wh * dp);
  ctx.setTransform(dp,0,0,dp,0,0);

  const lgobr = lgo.getBoundingClientRect();
  const cnvbr = cnv.getBoundingClientRect();
  lgo.style["left"]=( cnvbr.width- lgobr.width)+"px";
  lgo.style[ "top"]=(cnvbr.height-lgobr.height+18)+"px";
}

function handlekeyboard(e) {
  if (kAcceptedKeys.includes(e.code)) {
    sCurrentKeys.add(e.code);
    if (bEnableKeyboardDebug) {
      console.log(e.code);
    }
  }
}

function main() {
  const cnv = document.getElementById("arena");
  const dp  = window.devicePixelRatio;
  window.addEventListener("resize", resize);
  window.addEventListener("keydown", handlekeyboard);
  window.addEventListener("keyup", (e) => {
    sCurrentKeys.delete(e.code);
  });
  resize();
  draw();
}