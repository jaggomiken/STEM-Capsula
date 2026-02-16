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
#ifndef stemcapsulax_scene_box2d_h
#define stemcapsulax_scene_box2d_h

#include "stemcapsulax_scene.h"
#include "stemcapsulax_system.h"
#include "stemcapsulax_box2d_proxy.h"
#include "stemcapsulax_box2d_fromimage.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class SceneBox2D : public Scene {
  public:
    SceneBox2D();
    SceneBox2D(const SceneBox2D&)              = delete;
    SceneBox2D(SceneBox2D&&)                   = delete;
    SceneBox2D& operator=(const SceneBox2D&)   = delete;
    SceneBox2D& operator=(SceneBox2D&&)        = delete;
    virtual ~SceneBox2D();

    void clear() override;
    void trigger(uint32_t) override;
    void show() override;
    void draw(RenderTexture2D&, const SystemInfo&) override;
    void update() override;
    void unshow() override;

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_scene_box2d_h
