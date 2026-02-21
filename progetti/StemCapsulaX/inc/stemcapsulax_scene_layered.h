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
#ifndef stemcapsulax_scene_layered_h
#define stemcapsulax_scene_layered_h

#include "stemcapsulax_scene.h"
#include "stemcapsulax_layer.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * Questa classe rappresenta una scena che mostra una sequenza ordinata di
 * livelli, detti layer. Il primo livello, quello più lontano dagli occhi
 * dell'utente è quello di sfondo (background) e viene disegnato per primo.
 * Poi, a seguire, via via che ci si avvicina all'utente, vengono disegnati
 * gli altri livelli. Ogni livello ha la propria render-texture, per cui il
 * metodo draw() della scena si occupa unicamente di blittare le render
 * texture di ogni livello, nell'ordine di visualizzazione. In generale, il
 * numero predefinito di livelli è 2, un background e un foreground.
 * L'allocazione e deallocazione di un Layer non è gestita da questa classe.
 * Il Layer deve sopravvivere almeno finché non è rimosso dalla scena.
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class LayeredScene: public Scene {
  public:
    explicit LayeredScene(size_t szN_Layers = 2);
    LayeredScene(const LayeredScene&)              = delete;
    LayeredScene(LayeredScene&&)                   = delete;
    LayeredScene& operator=(const LayeredScene&)   = delete;
    LayeredScene& operator=(LayeredScene&&)        = delete;
    virtual ~LayeredScene();

    void layerAdd(Layer*);
    void layerRemove(size_t);

    size_t layerCount() const;
    bool isLayerNotNullAt(size_t) const;
    const Layer& layerAt(size_t) const;    
          Layer& layerAt(size_t);

    /* Questi metodi sono proxy verso gli omologhi di Layer */
    void clear() override final;
    void show() override final;
    void draw(RenderTexture2D&) override final;
    void update() override final;
    void unshow() override final;

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_scene_h
