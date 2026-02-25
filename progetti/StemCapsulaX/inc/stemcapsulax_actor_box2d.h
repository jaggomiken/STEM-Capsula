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
#ifndef stemcapsulax_actor_box2d_h
#define stemcapsulax_actor_box2d_h

#include "stemcapsulax_actor.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * Questa classe rappresenta un attore che è composto da corpi (body) di
 * Box2D e quindi è soggetto alla sua fisica. Può essere aggiunto ad un
 * layer di tipo Box2D.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class ActorBox2D : public Actor {
  public:
    explicit ActorBox2D(b2WorldId, const std::string& name = {});
    ActorBox2D(const ActorBox2D&)              = delete;
    ActorBox2D(ActorBox2D&&)                   = delete;
    ActorBox2D& operator=(const ActorBox2D&)   = delete;
    ActorBox2D& operator=(ActorBox2D&&)        = delete;
    virtual ~ActorBox2D();

    b2WorldId worldId() const;

    TypeID type() const override;
    void show() override;
    void hide() override;
    void update() override;
    void draw(RenderTexture2D&) override;
    void control(const ControlData&) override;

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_scene_h
