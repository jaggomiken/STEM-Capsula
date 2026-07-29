function grad_generic(ctx, obj, t, viewport) {
    const csx = viewport.csx;
    const csy = viewport.csy;
    const  xx = csx+obj.cx;
    const  yy = csy+obj.cy;
    const g = ctx.createLinearGradient(
        xx-obj.r,yy-obj.r,xx+obj.r,yy+obj.r);
    // const g = ctx.createRadialGradient(
    //     xx, yy, obj.r / 8
    //   , xx, yy, obj.r);
    g.addColorStop(0.0, rgba({r:255,g:  0,b:  0,a:0.4}));
    g.addColorStop(0.5, rgba({r:  0,g:  0,b:255,a:0.4}));
    g.addColorStop(1.0, rgba({r:  0,g:255,b:  0,a:0.4}));
    return g;
}

function draw_object(ctx, obj, t, viewport) {
    const csx = viewport.csx;
    const csy = viewport.csy;
    const xx = csx+obj.cx;
    const yy = csy+obj.cy;
    if (bEnableGlow) {
        ctx.save();
        ctx.shadowBlur  = obj.shadow.blur;
        ctx.shadowColor = 
        (null == obj.shadow.color) 
            ? hsla(obj.border): hsla(obj.shadow.color);
    }

    let g = null;
    if (null != obj.fn_gradmk) {
        g = obj.fn_gradmk(ctx, obj, t, viewport);
    }

    ctx.fillStyle   = (null != g) ? g : hsla(obj.fill);
    ctx.strokeStyle = hsla(obj.border);
    ctx.beginPath();
    ctx.arc(xx+obj.x,yy+obj.y,obj.r
        , 0, 2 * Math.PI, true);
    ctx.fill();
    ctx.stroke();
    ctx.strokeStyle = "white";
    ctx.beginPath();
    ctx.moveTo(xx-5, yy);
    ctx.lineTo(xx+5, yy)
    ctx.stroke();
    ctx.beginPath();
    ctx.moveTo(xx, yy-5);
    ctx.lineTo(xx, yy+5)
    ctx.stroke();
    ctx.strokeStyle = hsla({h:311, s: 81, l: 33, a: 0.6 });
    ctx.beginPath();
    ctx.moveTo(viewport.csx, viewport.csy);
    ctx.lineTo(xx,yy);
    ctx.stroke();
    if (bEnableGlow) { ctx.restore(); }
}

function draw_crossline(ctx, obj, t, viewport) {
    const csx = viewport.csx;
    const csy = viewport.csy;
    if (bEnableGlow) {
        ctx.save();
        ctx.shadowBlur  = obj.shadow.blur;
        ctx.shadowColor = 
        (null == obj.shadow.color) 
            ? rgba(obj.border) : rgba(obj.shadow.color);
    }
    ctx.fillStyle   = rgba(obj.fill);
    ctx.strokeStyle = rgba(obj.border);
    ctx.beginPath();
    ctx.moveTo(csx,       0);
    ctx.lineTo(csx, 2 * csy);
    ctx.stroke();
    ctx.beginPath();
    ctx.moveTo(      0, csy);
    ctx.lineTo(2 * csx, csy);
    ctx.stroke();
    if (bEnableGlow) { ctx.restore(); }
}

function draw_grid(ctx, obj, t, viewport) {
    const csx = viewport.csx;
    const csy = viewport.csy;
    const xs  = 32, ys = 32;
    if (bEnableGlow) {
        ctx.save();
        ctx.shadowBlur  = obj.shadow.blur;
        ctx.shadowColor = 
        (null == obj.shadow.color) 
            ? rgba(obj.border) : rgba(obj.shadow.color);
    }
    ctx.fillStyle   = rgba(obj.fill);
    ctx.strokeStyle = rgba(obj.border);
    
    const nc = 8.0 * viewport.w / xs;
    const nr = 9.0 * viewport.h / ys;
    
    ctx.beginPath();
    for (let n = 0;n < nc;++n) {
      ctx.moveTo(obj.cx+n*xs+obj.x,      obj.y);
      ctx.lineTo(obj.cx+n*xs+obj.x, viewport.h);
    }
    for (let n = 0;n < nr;++n) {
      ctx.moveTo(     obj.x, obj.cy+n*ys+obj.y);
      ctx.lineTo(viewport.w, obj.cy+n*ys+obj.y);
    }
    ctx.stroke();
    if (bEnableGlow) { ctx.restore(); }
}

function draw_player(ctx, obj, t, viewport) {
    const csx = viewport.csx;
    const csy = viewport.csy;
    const xx  = csx+obj.cx;
    const yy  = csy+obj.cy;
    if ((null != obj.image) && (1 === obj.imageloaded)) {
      ctx.save();
      ctx.shadowBlur  = 132;
      ctx.shadowColor = rgba({r: 120, g: 200, b: 180, a: 0.8});
      ctx.fillStyle   = rgba({r: 120, g: 120, b: 120, a: 0.5});
      ctx.beginPath();
      ctx.arc(xx+obj.x, yy+obj.y, 96
        , 0, 2 * Math.PI, true);
      ctx.fill();
      ctx.restore();
      ctx.drawImage(obj.image
        , xx+obj.x-64, yy+obj.y-64, 128, 128);
    } else {
      obj.image = new Image();
      obj.imageloaded = 0;      
      obj.image.src = obj.imagesrc;
      obj.image.onload = () => {
        obj.imageloaded = 1;
        console.log("IMAGE LOADED !");
        console.log(obj);
      };
    }
}

function object_make_axis() {
  return { cx: 0, cy: 0, x: 0, y: 0, r: 0
    , type: "AXIS", name: "cross", enabled: 1
    ,   shadow: { color: {r: 0, g: 255, b: 0, a: 0.6 }, blur: 20 }
    ,   border: {r: 255, g: 255, b: 255, a: 1.0 }
    ,     fill: {r:   0, g:   0, b:   0, a: 0.0 }
    , drawinfo: {} // stato del drawing
    , motiinfo: {} // stato della motion
    , motparam: {
           move_enable: 0
        , fx: 0.0, fy: 0.0, fR: 0.0
        , Ax:   0, Ay:   0, Ar: 120
        , color_enable: 0
        , fh: 0.0, fs:   0, fl:   0
        , Ah: 0.0, As:   0, Al:   0
        }
    , fn_motion: null
    ,   fn_draw: draw_crossline
    , fn_gradmk: null
  }  
}

function object_make_grid() {
  return { cx: 0, cy: 0, x: 0, y: 0, r: 0
    , type: "GRID", name: "grid", enabled: 1
    ,   shadow: { color: {r: 0, g: 255, b: 0, a: 0.1 }, blur: 0 }
    ,   border: {r: 255, g: 255, b: 255, a: 0.4 }
    ,     fill: {r:   0, g:   0, b:   0, a: 0.0 }
    , drawinfo: {} // stato del drawing
    , motiinfo: {} // stato della motion
    , motparam: {
           move_enable: 0
        , fx: 0.0, fy: 0.0, fR: 0.0
        , Ax:   0, Ay:   0, Ar: 120
        , color_enable: 0
        , fh: 0.0, fs:   0, fl:   0
        , Ah: 0.0, As:   0, Al:   0
        }
    , fn_motion: motion_grid
    ,   fn_draw: draw_grid
    , fn_gradmk: null
  }  
}

function object_make_ball(cx, cy, name, r) {
  return { cx: cx, cy: cy, x: 0, y: 0, r: r
    , type: "BALL", name: name, enabled: 1
    ,   shadow: { color: {h:291, s: 90, l: 80, a: 0.5 }, blur: 64 }
    ,   border: {h:   0, s: 100, l: 100, a: 0.8 }
    ,     fill: {h: 190, s:  91, l:  20, a: 0.5 }
    , drawinfo: {} // stato del drawing
    , motiinfo: {} // stato della motion
    , motparam: {
           move_enable: 1
        , fx: 1.0, fy: 1.0, fR: 0.0
        , Ax:  cx, Ay:  cy, Ar: r
        , color_enable: 1
        , fh: 0.0, fs: 1.0, fl: 1.0
        , Ah: 0.0, As:  50, Al:  80
        }
    , fn_motion: motion_object
    ,   fn_draw: draw_object
    , fn_gradmk: null
  }  
}
