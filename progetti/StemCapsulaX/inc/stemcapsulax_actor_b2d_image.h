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
#ifndef stemcapsulax_actor_b2d_image_h
#define stemcapsulax_actor_b2d_image_h

#include "stemcapsulax_actor_box2d.h"
#include "stemcapsulax_box2d_proxy.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * Questa classe rappresenta un attore che è composto da corpi box2d ciascuno
 * corrispondente ad un pixel di un'immagine caricata da disco.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class ActorB2DImage : public ActorBox2D {
  public:
    ActorB2DImage(b2WorldId, const b2Vec2& center, f32 scale = 1.0f
      , const std::string& name = {});
    ActorB2DImage(const ActorB2DImage&)              = delete;
    ActorB2DImage(ActorB2DImage&&)                   = delete;
    ActorB2DImage& operator=(const ActorB2DImage&)   = delete;
    ActorB2DImage& operator=(ActorB2DImage&&)        = delete;
    virtual ~ActorB2DImage();

    bool createFromImage(const std::string&);
    
    void update() override;
    void behave(u64,const std::vector<f32>&) override;
    void draw(RenderTexture2D&, Layer&) override;

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_actor_b2d_image_h
