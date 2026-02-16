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
#include "stemcapsulax_scene_manager.h"
#include "stemcapsulax_system.h"
#include <vector>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::SceneManager::Impl {
public:
  Impl();
 ~Impl();

  size_t sceneCount() const { return m_vScenes.size(); }
  void addScene(Scene* s) { m_vScenes.push_back(s); }
  void setCurrentSceneIndex(size_t value) { m_szCurrentSceneIndex = value; }
  size_t currentSceneIndex() const { return m_szCurrentSceneIndex; }
  Scene& sceneAtIndex(size_t szIndex) { return *m_vScenes.at(szIndex); }

private:
  std::vector<Scene*> m_vScenes;
  size_t m_szCurrentSceneIndex;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::SceneManager& stemcapsulax::SceneManager::GetInstance()
{
  static SceneManager factory;
  return factory;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::SceneManager::SceneManager()
: m_pImpl{ nullptr }
{
  m_pImpl = new(std::nothrow) Impl{};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::SceneManager::~SceneManager()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::SceneManager::hasCurrentScene() const
{
  size_t szCurIndex = m_pImpl->currentSceneIndex();
  return szCurIndex < m_pImpl->sceneCount();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Scene& stemcapsulax::SceneManager::currentScene()
{
  size_t szCurIndex = m_pImpl->currentSceneIndex();
  STEMCAPSULAX_CAPTURE_CPU(szCurIndex >= m_pImpl->sceneCount()
    , "Scene index overflow");
  return m_pImpl->sceneAtIndex(szCurIndex);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::SceneManager::switchToNextScene()
{
  size_t szCurIndex = m_pImpl->currentSceneIndex();
  if (++szCurIndex >= m_pImpl->sceneCount()) {
    szCurIndex = m_pImpl->sceneCount() - 1;
  }
  m_pImpl->setCurrentSceneIndex(szCurIndex);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::SceneManager::switchToPrevScene()
{
  size_t szCurIndex = m_pImpl->currentSceneIndex();
  if (--szCurIndex > m_pImpl->sceneCount()) { szCurIndex = 0; }
  m_pImpl->setCurrentSceneIndex(szCurIndex);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::SceneManager::setCurrentSceneByIndex(size_t szIndex)
{
  if (szIndex >= m_pImpl->sceneCount()) { return false; }
  m_pImpl->setCurrentSceneIndex(szIndex);
  return true;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
size_t stemcapsulax::SceneManager::currentSceneIndex() const
{
  return m_pImpl->currentSceneIndex();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
size_t stemcapsulax::SceneManager::sceneCount() const
{
  return m_pImpl->sceneCount();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::SceneManager::addScene(Scene* s)
{
  STEMCAPSULAX_CAPTURE_CPU(nullptr == s, "Pointer to scene is NULL");
  m_pImpl->addScene(s);
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::SceneManager::Impl::Impl()
: m_szCurrentSceneIndex { 0 }
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::SceneManager::Impl::~Impl()
{

}
