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
#ifndef stemcapsulax_actor_b2d_lab_h
#define stemcapsulax_actor_b2d_lab_h

#include "stemcapsulax_actor_box2d.h"
#include "stemcapsulax_box2d_proxy.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * Questa classe rappresenta un attore utile per sperimentare codice di
 * rendering particolare, per studiare RayLib dal punto di vista degli shader
 * e del disegno, nonché per approfondire OpenGL.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class ActorB2DLab : public ActorBox2D {
  public:
    ActorB2DLab(b2WorldId, const b2Vec2& center, f32 scale = 1.0f
      , const std::string& name = {});
    ActorB2DLab(const ActorB2DLab&)              = delete;
    ActorB2DLab(ActorB2DLab&&)                   = delete;
    ActorB2DLab& operator=(const ActorB2DLab&)   = delete;
    ActorB2DLab& operator=(ActorB2DLab&&)        = delete;
    virtual ~ActorB2DLab();

    void update() override;
    void draw(RenderTexture2D&, Layer&) override;

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_actor_b2d_lab_h
