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
#include "stemcapsulax_layer.h"
#include "stemcapsulax_system.h"
#include <map>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::Layer::Impl {
public:
  Impl();
 ~Impl();

  TaskRunner m_runner;
  std::map<std::string,Actor*> m_mapActors;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Layer::Layer()
: m_pImpl{ nullptr }
{
  m_pImpl = new(std::nothrow) Impl{};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Layer::~Layer()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::TaskRunner& stemcapsulax::Layer::runner()
{
  return m_pImpl->m_runner;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
const stemcapsulax::Actor* 
  stemcapsulax::Layer::actorByName(const std::string& name) const
{
  STEMCAPSULAX_CAPTURE_CPU(name.empty(), "Name is empty");
  if (0 != m_pImpl->m_mapActors.count(name)) {
    return m_pImpl->m_mapActors.at(name);
  }
  return nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Actor* 
  stemcapsulax::Layer::actorByName(const std::string& name)
{
  STEMCAPSULAX_CAPTURE_CPU(name.empty(), "Name is empty");
  if (0 != m_pImpl->m_mapActors.count(name)) {
    return m_pImpl->m_mapActors.at(name);
  }
  return nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::Layer::actorExists(const std::string& name) const
{
  STEMCAPSULAX_CAPTURE_CPU(name.empty(), "Name is empty");
  return (0 != m_pImpl->m_mapActors.count(name));
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::Layer::actorAdd(Actor* pA)
{
  STEMCAPSULAX_CAPTURE_CPU(nullptr == pA, "Pointer to actor is NULL");
  STEMCAPSULAX_CAPTURE_CPU(pA->name().empty(), "Actor name is empty");
  if (0 != m_pImpl->m_mapActors.count(pA->name())) {
    std::fprintf(stderr, "[ERROR]: Actor %s already in layer!\n"
      , pA->name().c_str());
    return false;
  }
  m_pImpl->m_mapActors.insert({ pA->name(), pA });
  return true;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::Layer::actorRemove(const std::string& name)
{
  STEMCAPSULAX_CAPTURE_CPU(name.empty(), "Actor name is empty");
  if (0 == m_pImpl->m_mapActors.count(name)) {
    std::fprintf(stderr, "[ERROR]: Actor %s not found in layer!\n"
      , name.c_str());
    return false;
  }
  m_pImpl->m_mapActors.erase(name);
  return true;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
size_t stemcapsulax::Layer::actorCout() const
{
  return m_pImpl->m_mapActors.size();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::Layer::enumerate(const std::function<void(Actor*)>& cb)
{
  if (cb) {
    for (auto& item : m_pImpl->m_mapActors) {
      if (nullptr != item.second) {
        cb(item.second);
      }
    }
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Layer::Impl::Impl()
{

}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::Layer::Impl::~Impl()
{

}
