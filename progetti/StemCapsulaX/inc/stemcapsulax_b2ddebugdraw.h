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
#ifndef stemcapsulax_b2ddebugdraw_h
#define stemcapsulax_b2ddebugdraw_h

#include "stemcapsulax_system.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * MACROS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define STEMCAPSULAX_B2DDD_DEBUG                                           0

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * MACROS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define TX(c)\
  ((c) * (clz->fPixelToUnitRatio))
#define TY(c)\
  ((c) * (clz->fPixelToUnitRatio))
#define RC(color)\
  Color{\
      uint8_t((u32(color) & 0x00FF0000) >> 16)\
    , uint8_t((u32(color) & 0x0000FF00) >>  8)\
    , uint8_t((u32(color) & 0x000000FF)      )\
    , 255}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class RaylibBox2DDebugDraw {
  public:
    f32 fPixelToUnitRatio;
    f32 fScreenWidth;
    f32 fScreenHeight;
    buffer<Vector2,8192> v2buffer;
    
    RaylibBox2DDebugDraw() noexcept {
      m_dd = b2DefaultDebugDraw();
      m_dd.context = this;
      m_dd.drawShapes           = true;   // true
      m_dd.drawJoints           = true;   // true
      m_dd.drawMass             = false;  // false
      m_dd.drawContacts         = false;   // true
      m_dd.useDrawingBounds     = false;  // se true, crea problemi
      m_dd.drawJointExtras      = false;  // false
      m_dd.drawBounds           = false;  // false
      m_dd.drawBodyNames        = false;  // false
      m_dd.drawGraphColors      = false;  // false
      m_dd.drawContactNormals   = false;   // true
      m_dd.drawContactImpulses  = false;   // true
      m_dd.drawContactFeatures  = false;   // true
      m_dd.drawFrictionImpulses = false;   // true
      m_dd.drawIslands          = false;  // false
      m_dd.DrawPolygonFcn      = &RaylibBox2DDebugDraw::m_DrawPolygon;
      m_dd.DrawSolidPolygonFcn = &RaylibBox2DDebugDraw::m_DrawSolidPolygon;
      m_dd.DrawCircleFcn       = &RaylibBox2DDebugDraw::m_DrawCircle;
      m_dd.DrawSolidCircleFcn  = &RaylibBox2DDebugDraw::m_DrawSolidCircle;
      m_dd.DrawSolidCapsuleFcn = &RaylibBox2DDebugDraw::m_DrawSolidCapsule;
      m_dd.DrawSegmentFcn      = &RaylibBox2DDebugDraw::m_DrawSegment;
      m_dd.DrawTransformFcn    = &RaylibBox2DDebugDraw::m_DrawTransform;
      m_dd.DrawPointFcn        = &RaylibBox2DDebugDraw::m_DrawPoint;
      m_dd.DrawStringFcn       = &RaylibBox2DDebugDraw::m_DrawString;
    }

    void drawWorld(b2WorldId wid) noexcept { b2World_Draw(wid, &m_dd); }

  private:
    static void
      m_DrawPolygon(const b2Vec2* vertices, int vertexCount
        , b2HexColor color, void* context) {
      if ((nullptr == vertices) || (vertexCount < 2)) { return; }
#if STEMCAPSULAX_B2DDD_DEBUG == 1
      std::printf("[B2DDD]: DrawPolygon\n");
#endif
      auto* clz = reinterpret_cast<RaylibBox2DDebugDraw*>(context);
      Color c = RC(color);
      for (int j = 0;j < vertexCount;++j) {
        if (j < (vertexCount - 1)) {
          DrawLine(
              TX(vertices[j  ].x), TY(vertices[j  ].y)
            , TX(vertices[j+1].x), TY(vertices[j+1].y)
            , c);
        } else {
          DrawLine(
              TX(vertices[j].x), TY(vertices[j].y)
            , TX(vertices[0].x), TY(vertices[0].y)
            , c);
        }
      }
    }
    static void
      m_DrawSolidPolygon(b2Transform transform
        , const b2Vec2* vertices, int vertexCount
        , float radius, b2HexColor color, void* context) {
      if ((nullptr == vertices) || (vertexCount < 3)) { return; }
#if STEMCAPSULAX_B2DDD_DEBUG == 1
      std::printf("[B2DDD]: DrawSolidPolygon (radius=%f, vc=%d)\n"
        , radius, vertexCount);
#endif
      auto* clz = reinterpret_cast<RaylibBox2DDebugDraw*>(context);
      Color c = RC(color);
      clz->v2buffer.count = 0;
      for (int j = 0;j < vertexCount;++j) {
        auto p = b2TransformPoint(transform, vertices[j]);
        clz->v2buffer.setItemAt(j, { TX(p.x), TY(p.y) });
        clz->v2buffer.count++;
      }
      for (int j = 1;j < (vertexCount - 1);++j) {
        DrawTriangle( // i vertici vanno dati in senso anti-orario
            clz->v2buffer.itemAt(j+1)
          , clz->v2buffer.itemAt(j  )
          , clz->v2buffer.itemAt(0  )
          , Fade(c, .7f));
      }
      for (int j = 0;j < clz->v2buffer.count;++j) {
        const auto& p0 = clz->v2buffer.itemAt((j  ));
        const auto& p1 = clz->v2buffer.itemAt((j+1) % vertexCount);
        DrawLineV(p0, p1, c);
      }
    }
    static void
      m_DrawCircle(b2Vec2 center, float radius
        , b2HexColor color, void* context) {
      if (radius <= .0f) { return; }
#if STEMCAPSULAX_B2DDD_DEBUG == 1
      std::printf("[B2DDD]: DrawCircle (radius=%f)\n", radius);
#endif
      auto* clz = reinterpret_cast<RaylibBox2DDebugDraw*>(context);
      DrawCircleLines(TX(center.x), TY(center.y), TX(radius), RC(color));
    }
    static void
      m_DrawSolidCircle(b2Transform transform
        , float radius, b2HexColor color, void* context) {
      if (radius <= .0f) { return; }
#if STEMCAPSULAX_B2DDD_DEBUG == 1
      std::printf("[B2DDD]: DrawSolidCircle (radius=%f)\n", radius);
#endif
      auto* clz = reinterpret_cast<RaylibBox2DDebugDraw*>(context);
      auto c = RC(color);
      auto pc = b2TransformPoint(transform, {    .0f, .0f });
      auto rc = b2TransformPoint(transform, { radius, .0f });
      auto cn = Vector2{ TX(pc.x), TY(pc.y) };
      auto rn = Vector2{ TX(rc.x), TY(rc.y) };
      DrawCircle(cn.x, cn.y, TX(radius), Fade(c, 0.7f));
      DrawLineV(cn, rn, c);
    }
    static void
      m_DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius
        , b2HexColor color, void* context) {
      if (radius <= .0f) { return; }
#if STEMCAPSULAX_B2DDD_DEBUG == 1
      std::printf("[B2DDD]: DrawSolidCapsule (radius=%f)\n", radius);
#endif
      auto* clz = reinterpret_cast<RaylibBox2DDebugDraw*>(context);
      auto c1 = Vector2{ TX(p1.x), TY(p1.y) };
      auto c2 = Vector2{ TX(p2.x), TY(p2.y) };
      auto  r = TX(radius);
      auto  c = RC(color);
      DrawCircleV(c1, r, Fade(c, 0.7f));
      DrawCircleV(c2, r, Fade(c, 0.7f));
      DrawLineV(c1, c2, c);
      f32 beta = std::atan2f(c1.y - c2.y, c1.x - c2.x);
      f32 dx = r * std::sinf(beta), dy = r * std::cosf(beta);
      Vector2 v[] = {
          { c1.x + dx, c1.y - dy }
        , { c1.x - dx, c1.y + dy }
        , { c2.x + dx, c2.y - dy }
        , { c2.x - dx, c2.y + dy }
      };
      DrawTriangle(v[0], v[2], v[1], Fade(c, .7f));
      DrawTriangle(v[3], v[1], v[2], Fade(c, .7f));
      DrawLineV(v[0], v[1], c);
      DrawLineV(v[0], v[2], c);
      DrawLineV(v[3], v[1], c);
      DrawLineV(v[3], v[2], c);
    }
    static void
      m_DrawSegment(b2Vec2 p1, b2Vec2 p2
        , b2HexColor color, void* context) {
#if STEMCAPSULAX_B2DDD_DEBUG == 1
      std::printf("[B2DDD]: DrawSegment\n");
#endif
      auto* clz = reinterpret_cast<RaylibBox2DDebugDraw*>(context);
      DrawLineV({ TX(p1.x), TY(p1.y) }, { TX(p2.x), TY(p2.y) }, RC(color));
    }
    static void
      m_DrawTransform(b2Transform transform, void* context) {
#if STEMCAPSULAX_B2DDD_DEBUG == 1
      std::printf("[B2DDD]: DrawTransform\n");
#endif
      auto* clz = reinterpret_cast<RaylibBox2DDebugDraw*>(context);
    }
    static void
      m_DrawPoint(b2Vec2 p, float size
        , b2HexColor color, void* context) {
#if STEMCAPSULAX_B2DDD_DEBUG == 1      
      std::printf("[B2DDD]: DrawPoint\n");
#endif
      auto* clz = reinterpret_cast<RaylibBox2DDebugDraw*>(context);
      DrawCircle(TX(p.x), TY(p.y), 3.0f, RC(color));
    }
    static void
      m_DrawString(b2Vec2 p, const char* s
        , b2HexColor color, void* context) {
#if STEMCAPSULAX_B2DDD_DEBUG == 1
      std::printf("[B2DDD]: DrawString\n");
#endif
      auto* clz = reinterpret_cast<RaylibBox2DDebugDraw*>(context);
      DrawText(s, TX(p.x), TY(p.y), 12, RC(color));
    }

    b2DebugDraw m_dd;
  };
}

#endif // stemcapsulax_b2ddebugdraw_h
