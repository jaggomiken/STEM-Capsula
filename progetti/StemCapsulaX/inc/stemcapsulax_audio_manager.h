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
#ifndef stemcapsulax_audio_manager_h
#define stemcapsulax_audio_manager_h

#include "stemcapsulax_system.h"

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * TYPES
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
using audiosamplebuffer48x2 = stemcapsulax::buffer<f32, 48000 * 2>;

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * CLASS DECLARATION
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
namespace stemcapsulax {
  class AudioManager {
  public:
    using DataCallback = 
      std::function<void(
          f32 fpercdone
        , const std::vector<f32>& vleft
        , const std::vector<f32>& vrght
        , f32 fL_Energy
        , f32 fR_Energy
        , std::pair<f32,f32> pairFreqAmpMinLft
        , std::pair<f32,f32> pairFreqAmpMaxLft
        , std::pair<f32,f32> pairFreqAmpMinRgt
        , std::pair<f32,f32> pairFreqAmpMaxRgt
      )>;

    struct StatusData {
      u32 uNTotalFrames;         // numero totale di frame
      u32 uNStreamedSamples;     // numero di campioni trasferiti al device
      u32 uFTQueueSizeInSamples; // numero campioni accodati per Fourier
      u32 uNFTDequeuedSamples;   // numero di campioni rimossi dalla coda FFT
      u32 uNFTProcessedSamples;  // numero di campioni elaborati dalla FFT
      u32 uNFFTReadyResults;     // numero di risultati pronti in coda FFT
    };
  
    static AudioManager& GetInstance();
   ~AudioManager();

    // La callback è invocata nel metodo update quando sono disponibili dati FFT
    // (viene invocata nel main thread)
    void registerDataCallback(const DataCallback&);

    void reset();
    void update();
    void shutdown();

    std::string mainWaveFilename() const;
    bool loadMainWave(const std::string&);
    void playMainWave(bool);
    bool isMainWavePlaying() const;
    bool isMainWavePlayable() const;

    const StatusData& statusData() const;

  protected:
    AudioManager();
    AudioManager(const AudioManager&)            = delete;
    AudioManager(AudioManager&&)                 = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    AudioManager& operator=(AudioManager&&)      = delete;

  private:
    class Impl; Impl* m_pImpl;
  };
}

#endif // stemcapsulax_audio_manager_h
