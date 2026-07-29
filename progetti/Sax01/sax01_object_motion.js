let Axx = 1.0, Ayy = 1.0;

function motion_object(obj, t, viewport) {
  /* INTERESSANTE DA MIGLIORARE
  if (sCurrentKeys.has( "ArrowLeft")) { Axx -= 1.0; }
  if (sCurrentKeys.has("ArrowRight")) { Axx += 1.0; }
  if (sCurrentKeys.has(   "ArrowUp")) { Ayy -= 1.0; }
  if (sCurrentKeys.has( "ArrowDown")) { Ayy += 1.0; }
  */
  if (1 === obj.motparam.move_enable) {
    obj.x = Axx * (Math.cos(2 * Math.PI * 8.0 * t));
    obj.y = Ayy * (Math.cos(2 * Math.PI * 4.0 * t));

    obj.cx = obj.motparam.Ax * (Math.cos(2 * Math.PI 
      * obj.motparam.fx * t));
    obj.cy = obj.motparam.Ay * (Math.sin(2 * Math.PI 
      * obj.motparam.fy * t));
    obj.r  = obj.motparam.Ar * (Math.cos(2 * Math.PI
      * obj.motparam.fR * t) + 1.1);
  }
  if (1 === obj.motparam.color_enable) {
    obj.fill.h = obj.motparam.Ah * (Math.cos(2 * Math.PI
      * obj.motparam.fh * t) + 1) * 0.5;
    obj.fill.s = obj.motparam.As * (Math.sin(2 * Math.PI
      * obj.motparam.fs * t) + 1) * 0.5;
    obj.fill.l = obj.motparam.Al * (Math.sin(2 * Math.PI
      * obj.motparam.fl * t) + 1) * 0.5;
  }
}

function motion_gravity(obj, t, viewport) {
    if (1 === obj.motparam.move_enable) {
        const g = 9.8 * 100.0, v0 = 0, y0 = -viewport.h / 2;
        obj.cy = g * 0.5 * t * t + v0 * t + y0;
        if (obj.cy >= viewport.csy) {
        obj.cy = viewport.csy;
        }
    }
    if (1 === obj.motparam.color_enable) {
        obj.fill.h = obj.motparam.Ah * (Math.cos(2 * Math.PI
        * obj.motparam.fh * t) + 1) * 0.5;
        obj.fill.s = obj.motparam.As * (Math.sin(2 * Math.PI
        * obj.motparam.fs * t) + 1) * 0.5;
        obj.fill.l = obj.motparam.Al * (Math.sin(2 * Math.PI
        * obj.motparam.fl * t) + 1) * 0.5;
    }
}

function motion_grid(obj, t, viewport) {
  let dx = 0, dy = 0;

  if (sCurrentKeys.has( "ArrowLeft")) { dx = -1; }
  if (sCurrentKeys.has("ArrowRight")) { dx = +1; }
  if (sCurrentKeys.has(   "ArrowUp")) { dy = -1; }
  if (sCurrentKeys.has( "ArrowDown")) { dy = +1; }

  obj.cx += dx;
  obj.cy += dy;

  obj.x = 8.0 * Math.cos(2 * Math.PI * t);
  obj.y = 8.0 * Math.sin(2 * Math.PI * t);
}

function motion_player(obj, t, viewport) {
  let dx = 0, dy = 0;

  if (sCurrentKeys.has( "ArrowLeft")) { dx = -4; }
  if (sCurrentKeys.has("ArrowRight")) { dx = +4; }
  if (sCurrentKeys.has(   "ArrowUp")) { dy = -4; }
  if (sCurrentKeys.has( "ArrowDown")) { dy = +4; }

  obj.cx += dx;
  obj.cy += dy;

  obj.x = 8.0 * Math.cos(2 * Math.PI * 1.0 * t);
  obj.y = 8.0 * Math.sin(2 * Math.PI * 1.0 * t);
}