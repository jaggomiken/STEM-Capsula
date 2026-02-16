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
#ifndef stemcapsulax_scene_manager_h
#define stemcapsulax_scene_manager_h

#include "stemcapsulax_scene.h"
#include "stemcapsulax_system.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class SceneManager {
  public:
    static SceneManager& GetInstance();
   ~SceneManager();

    bool hasCurrentScene() const;
    Scene& currentScene();
    void switchToNextScene(); // commuta sulla scena e la attiva (show)
    void switchToPrevScene(); // commuta sulla scena e la attiva (show)
    bool setCurrentSceneByIndex(size_t); // commuta ma non attiva
    size_t currentSceneIndex() const;
    
    size_t sceneCount() const;
    void addScene(Scene*);

  protected:
    SceneManager();
    SceneManager(const SceneManager&)            = delete;
    SceneManager(SceneManager&&)                 = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager& operator=(SceneManager&&)      = delete;

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_scene_manager_h
