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
#ifndef stemcapsulax_actor_puppet_h
#define stemcapsulax_actor_puppet_h

#include "stemcapsulax_actor_box2d.h"
#include "stemcapsulax_box2d_proxy.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * Questa classe rappresenta un attore che è composto da corpi (body) di
 * Box2D e quindi è soggetto alla sua fisica. Può essere aggiunto ad un
 * layer di tipo Box2D.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class ActorPuppet : public ActorBox2D {
  public:
    /* potenza di due così si possono mettere in OR */
    enum class Behaviour : u64 {
        kOPENLEGS   = 0x00000000000000001LLU
      , kOPENARMS   = 0x00000000000000002LLU
      , kJUMP       = 0x00000000000000004LLU
      , kTREMBLE    = 0x00000000000000008LLU
      , kMOVELEGS   = 0x00000000000000010LLU
      , kLEFTARMUP  = 0x00000000000000020LLU
      , kRIGHTARMUP = 0x00000000000000040LLU
    };

    /* Opzioni */
    enum class Options : u08 {
        kNOGROUNDBASE
      , kGROUNDBASENOWALLS
      , kGROUNDBASEWWALLS
    };

    ActorPuppet(b2WorldId, const b2Vec2& center, f32 scale = 1.0f
      , const std::string& name = {}
      , Options = Options::kNOGROUNDBASE
      , const std::vector<Color>& vcolors = {});
    ActorPuppet(const ActorPuppet&)              = delete;
    ActorPuppet(ActorPuppet&&)                   = delete;
    ActorPuppet& operator=(const ActorPuppet&)   = delete;
    ActorPuppet& operator=(ActorPuppet&&)        = delete;
    virtual ~ActorPuppet();

    void moveRelative(f32 x, f32 y);

    void update() override;
    void behave(u64,const std::vector<f32>&) override;
    void draw(RenderTexture2D&, Layer&) override;
    
  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_actor_puppet_h
