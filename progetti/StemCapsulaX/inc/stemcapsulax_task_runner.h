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
#ifndef stemcapsulax_task_runner_h
#define stemcapsulax_task_runner_h

#include "stemcapsulax_system.h"
#include <functional>
#include <string>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * Questa classe gestisce dei semplici task in modo mono-thread e non thread
 * safe. Il suo scopo è girare in una scena e in ogni singolo layer per
 * gestire funzionalità atomiche fornite da un Director. Il task è una
 * struct con il functor update() per gestire input e stato, e poi il
 * functor draw() per gestire eventuale disegno. Il layer chiama la draw()
 * del functor nella sua draw() e update() nella sua update(). Lo stesso
 * fa una scena.
 * 
 * I task sono memorizzati in ordine alfabetico e quindi saranno eseguiti
 * in tale ordine. Si consiglia dunque di scegliere nomi che rispecchino, se
 * necessario, un ordine alfabetico di esecuzione. L'idea è che sia la scena
 * sia ogni layer dispongano di un runner al proprio interno.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class TaskRunner {
  public:
    struct Task;
    using UpdateFunc = std::function<void(TaskRunner&, Task&)>;
    using   DrawFunc = std::function<void(TaskRunner&, Task&
      , RenderTexture2D&)>;
    struct Task {
      std::string name;
      void* pUserData;
      UpdateFunc update;
      DrawFunc draw;
    };

    TaskRunner();
    TaskRunner(const TaskRunner&)              = delete;
    TaskRunner(TaskRunner&&)                   = delete;
    TaskRunner& operator=(const TaskRunner&)   = delete;
    TaskRunner& operator=(TaskRunner&&)        = delete;
    ~TaskRunner();

    void taskAdd(const Task&);
    void taskRemove(const std::string&);
    size_t taskCount() const;
    void enumerate(const std::function<void(Task&)>&);

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_scene_h
