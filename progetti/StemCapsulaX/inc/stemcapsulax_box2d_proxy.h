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
#ifndef stemcapsulax_box2d_proxy_h
#define stemcapsulax_box2d_proxy_h

#include "stemcapsulax_system.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * BOX2D RATIO FUNCTIONS
 * box2d lavora bene con corpi tra 0.1 e 10 metri e per i corpi fissi, fino a
 * 50 metri, quindi non si può lavorare in pixel perché la simulazione ne
 * risente (come documentato sul sito). Il mondo non deve essere superiore a
 * 12km.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  struct Conv {
    static Conv& GetInstance();
    f32 fScreenWidth;
    f32 fScreenHeight;
    f32 fWorldWidth;
    f32 fWorldHeight;
    f32 x_w2s(f32 xw) { return ( fScreenWidth /   fWorldWidth) * xw; }
    f32 y_w2s(f32 yw) { return (fScreenHeight /  fWorldHeight) * yw; }
    f32 x_s2w(f32 xs) { return (  fWorldWidth /  fScreenWidth) * xs; }
    f32 y_s2w(f32 ys) { return ( fWorldHeight / fScreenHeight) * ys; }
  };
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * USER COMMAND
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  struct UserCommand {
    static constexpr uint32_t kUP    = 0x00000001;
    static constexpr uint32_t kDOWN  = 0x00000002;
    static constexpr uint32_t kLEFT  = 0x00000004;
    static constexpr uint32_t kRIGHT = 0x00000008;
    static constexpr uint32_t kFIRE1 = 0x00000010;
    static constexpr uint32_t kFIRE2 = 0x00000020;
    static constexpr uint32_t kROTCW = 0x00000040;
    static constexpr uint32_t kROTCC = 0x00000080;
  };
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class Box2DProxy {
  public:
    static void 
      RemoveBodiesOutsideRect(f32 x0, f32 y0, f32 x1, f32 y1
        , std::vector<b2BodyId>& vbodies);

    Box2DProxy();
   ~Box2DProxy();

    void setWidth(f32 value);
    f32 width() const;

    void setHeight(f32 value);
    f32 height() const;

    void setUserCommand(uint32_t); // bitmask di UserCommand in OR
    uint32_t currentUserCommand() const;

    b2WorldId worldId() const;
    f32 pixelPerMeter() const;

    void update(); // esegue step e fisica
    void destroyInactiveBodies();

    void createBodyTestGround(Color);
    void createBodyConcaveGround(Color);
    void createBodyGroundNoWalls(Color);
    void createBodyGroundRect(f32 x, f32 y, f32 w, f32 h,Color);
    void createBodyRectangle(f32 x, f32 y, f32 w, f32 h,Color);
    void createBodyCircle(f32 x, f32 y, f32 r,Color);
    void createBodyPolygon(f32 x, f32 y, const b2Vec2*, size_t count,Color);
    void createBodyCapsule(f32 x, f32 y, const b2Vec2& p0, const b2Vec2& p1, f32 r,Color);
    void createExplosion(f32 x, f32 y, f32 energy);
    void createBodyFromImage(const std::string&);
    void removeBodiesOutsideRect(f32 x0, f32 y0, f32 x1, f32 y1);
    void removeBodiesYGreaterThan(f32 y);
    void removeBodiesXGreaterThan(f32 y);

  protected:
    Box2DProxy(const Box2DProxy&)            = delete;
    Box2DProxy(Box2DProxy&&)                 = delete;
    Box2DProxy& operator=(const Box2DProxy&) = delete;
    Box2DProxy& operator=(Box2DProxy&&)      = delete;

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_box2d_proxy_h
