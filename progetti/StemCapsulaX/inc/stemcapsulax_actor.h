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
#ifndef stemcapsulax_actor_h
#define stemcapsulax_actor_h

#include "stemcapsulax_system.h"
#include <functional>
#include <string>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * Questa classe rappresenta un attore che compare e agisce in un layer. Si
 * differenzia da un Task perché è più complesso in termini di comportamenti
 * e, per esempio in un layer Box2D, può essere composta da molti body tra
 * loro interconnessi.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class Actor {
  public:
    using TypeID = u32;
    struct ControlData {
      // da sistemare prossimamente
    };

    explicit Actor(const std::string& name = {});
    Actor(const Actor&)              = delete;
    Actor(Actor&&)                   = delete;
    Actor& operator=(const Actor&)   = delete;
    Actor& operator=(Actor&&)        = delete;
    virtual ~Actor();

    std::string name() const;

    virtual TypeID type() const = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void update() = 0;
    virtual void draw(RenderTexture2D&) = 0;
    virtual void control(const ControlData&) = 0;
    virtual void behave(u64,const std::vector<f32>&) = 0;

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_scene_h
