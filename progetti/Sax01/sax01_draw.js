function draw() {
    const cnv = document.getElementById("arena");
    const dp  = window.devicePixelRatio;
    const ctx = cnv.getContext("2d");
    const 
          csx =  cnv.width / (2 * dp)
        , csy = cnv.height / (2 * dp);
    const viewport = { 
            x: 0,         y: 0
        ,   w: cnv.width, h: cnv.height
        , csx: csx,     csy: csy 
    };
    if (bEnableClearRect) {
        ctx.clearRect(0, 0, cnv.width, cnv.height);
    }
    if (bEnableLighter) {
        ctx.globalCompositeOperation = "lighter";
    }
    for (let k in vobjects) {
        const obj = vobjects[k];
        if (1 === obj.enabled) {
        if (null != obj.fn_motion) { 
            obj.fn_motion(obj, t, viewport);
        }
        if (null != obj.fn_draw) {
            obj.fn_draw(ctx, obj, t, viewport);
        }
        }
    }
    t += kFD;
    const dt = performance.now() - AppStatus.t0;
    if (dt >= 1000) {
      AppStatus.fps = AppStatus.nframes;
      AppStatus.nframes = 0;
      AppStatus.t0 = performance.now();
      const lblfps = document.getElementById("fps");
      lblfps.textContent = AppStatus.fps + "fps";
    } else {
      AppStatus.nframes++;
    }
    requestAnimationFrame(draw);
}