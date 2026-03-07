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
#include "stemcapsulax_audio_manager.h"
#include <queue>
#include <mutex>
#include <limits>
#include <thread>
#include <fft_stereo_simd.h>

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * MACROS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
#define STEMCAPSULAX_AUDIOMANAGER_BUFSZ                                4096U
#define STEMCAPSULAX_AUDIOMANAGER_FFTSZ                                4096U
#define STEMCAPSULAX_AUDIOMANAGER_FRQHZ                               48000U
#define STEMCAPSULAX_AUDIOMANAGER_REALTIME                                1U

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * LOCAL FUNCTIONS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
static void AudioStreamCallback48x2(void*,u32); // prototipo di RAYLIB
static struct AudioStreamCallbackStatus {
  f32* pAudioSamples;
  i32 iAudioWindowCursor;
  i32 iFramesCount;
  i32 iAudioBufferSize;
  std::queue<audiosamplebuffer48x2> qdata;
  std::mutex mtxqdata;
} gASCS;

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * LOCAL STRUCT
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
struct FFTResultDataPackage {
  fftsimd::SpectrumResult res;
  f32 fL_Energy;
  f32 fR_Energy;
  std::pair<f32,f32> pairFreqAmpMinLft;
  std::pair<f32,f32> pairFreqAmpMaxLft;
  std::pair<f32,f32> pairFreqAmpMinRgt;
  std::pair<f32,f32> pairFreqAmpMaxRgt;
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * PRIVATE IMPLEMENTATION CLASS
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
class stemcapsulax::AudioManager::Impl {
public:
  Impl();
 ~Impl();

  Wave m_waveMain;
  AudioStream m_streamMainWave;
  bool m_bMainWavePlaying;
  f32 m_fMainWaveSampleMax;
  f32 m_fMainWaveSampleMin;
  f32 m_fMainWaveSampleAvg;
  std::string m_strMainWaveFilename;
  
  std::queue<FFTResultDataPackage> m_qfftres;
  std::mutex m_mtxqfftres;

  StatusData m_data;
  std::thread m_threadFFT;
  bool m_bExitThread;
  bool m_bThreadStarted;
  DataCallback m_datacb;

  void m_ComputeEnergyForResult(const fftsimd::SpectrumResult&
    , FFTResultDataPackage& dataout);
};

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * STATIC METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::AudioManager& stemcapsulax::AudioManager::GetInstance()
{
  static AudioManager am; return am;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::AudioManager::AudioManager()
: m_pImpl{ nullptr }
{
  m_pImpl = new(std::nothrow) Impl{};
  STEMCAPSULAX_CAPTURE_CPU(nullptr == m_pImpl, "Cannot allocate");
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::AudioManager::~AudioManager()
{
  delete m_pImpl;
  m_pImpl = nullptr;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::AudioManager::registerDataCallback(const DataCallback& cb)
{
  m_pImpl->m_datacb = cb;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::AudioManager::shutdown()
{
  playMainWave(false);
  CloseAudioDevice();
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::AudioManager::reset()
{
  m_pImpl->m_datacb = {};
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::AudioManager::update()
{
  m_pImpl->m_data.uNStreamedSamples     = u32(gASCS.iAudioWindowCursor);
  m_pImpl->m_data.uFTQueueSizeInSamples = u32(gASCS.qdata.size());
  { std::lock_guard<std::mutex> guard{ m_pImpl->m_mtxqfftres };
    m_pImpl->m_data.uNFFTReadyResults = u32(m_pImpl->m_qfftres.size());
#if STEMCAPSULAX_AUDIOMANAGER_REALTIME == 1
    if (m_pImpl->m_qfftres.size() >= 2) {
      std::fprintf(stdout, "[AUDIOMANAGER]: REALTIME, discarding queue...\n");
      while (m_pImpl->m_qfftres.size() > 1) {
        m_pImpl->m_qfftres.pop();
      }
    }
#endif    
    if (!m_pImpl->m_qfftres.empty()) {
      const auto& item = m_pImpl->m_qfftres.front();
      if (m_pImpl->m_datacb) {
        f32 fperc = 100.0f * f32(m_pImpl->m_data.uNStreamedSamples) 
          / f32(2.0f * m_pImpl->m_data.uNTotalFrames);
        m_pImpl->m_datacb(fperc, item.res.magL, item.res.magR
          , item.fL_Energy, item.fR_Energy
          , item.pairFreqAmpMinLft, item.pairFreqAmpMaxLft
          , item.pairFreqAmpMinRgt, item.pairFreqAmpMaxRgt);
      }
      m_pImpl->m_qfftres.pop();
    }
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
std::string stemcapsulax::AudioManager::mainWaveFilename() const
{
  return m_pImpl->m_strMainWaveFilename;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::AudioManager::loadMainWave(const std::string& filename)
{
  // Imposta lo stato dell'oggetto e istruisce RAYLIB sullo stream
  m_pImpl->m_strMainWaveFilename = filename;
  m_pImpl->m_waveMain = LoadWave(filename.c_str());
  m_pImpl->m_streamMainWave = LoadAudioStream(
      m_pImpl->m_waveMain.sampleRate
    , m_pImpl->m_waveMain.sampleSize
    , m_pImpl->m_waveMain.channels);
  SetAudioStreamCallback(m_pImpl->m_streamMainWave, AudioStreamCallback48x2);
  if (!filename.empty()) {
    std::printf(
        "[AUDIOMANAGER]: Filename %s\n"
        "  (SampleRate %u, SampleSize %u, NChannels %u)\n", filename.c_str()
      , m_pImpl->m_waveMain.sampleRate
      , m_pImpl->m_waveMain.sampleSize
      , m_pImpl->m_waveMain.channels);
  }
  // Calcola informazioni sui campioni (max, min, ecc.)
  m_pImpl->m_fMainWaveSampleMax = .0f;
  m_pImpl->m_fMainWaveSampleMin = std::numeric_limits<f32>::max();
  m_pImpl->m_fMainWaveSampleAvg = .0f;
  f64 faccum = .0;
  for (i32 j = 0;j < m_pImpl->m_waveMain.frameCount;++j) {
    f32 s = reinterpret_cast<const f32*>(m_pImpl->m_waveMain.data)[j];
    m_pImpl->m_fMainWaveSampleMax = std::max<f32>(
        m_pImpl->m_fMainWaveSampleMax, s);
    m_pImpl->m_fMainWaveSampleMin = std::min<f32>(
        m_pImpl->m_fMainWaveSampleMin, s);
    faccum += f64(s);
  }
  if (0 != m_pImpl->m_waveMain.frameCount) {
    m_pImpl->m_fMainWaveSampleAvg =
      f32(faccum / f64(m_pImpl->m_waveMain.frameCount));
    std::printf("[AUDIOMANAGER]:  NFrames %u Min %.6f Max %.6f Avg %.6f\n"
      , m_pImpl->m_waveMain.frameCount
      , m_pImpl->m_fMainWaveSampleMin
      , m_pImpl->m_fMainWaveSampleMax
      , m_pImpl->m_fMainWaveSampleAvg);
  }
  else {
    std::printf("[AUDIOMANAGER]:  NO AUDIO FILE LOADED.\n");
  }
  
  // imposta lo stato iniziale
  m_pImpl->m_data.uNTotalFrames = u32(m_pImpl->m_waveMain.frameCount);

  // Configura la struttura localmente globale gASCS
  gASCS.pAudioSamples      = reinterpret_cast<float*>(m_pImpl->m_waveMain.data);
  gASCS.iAudioWindowCursor = 0; // questo lavora su x2 (stereo)
  gASCS.iFramesCount       = m_pImpl->m_waveMain.frameCount * 2; // stereo
  gASCS.iAudioBufferSize   = STEMCAPSULAX_AUDIOMANAGER_BUFSZ;
  return true; // Gestire meglio l'errore in questa funzione
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::AudioManager::playMainWave(bool bPlayOrPause)
{
  if (bPlayOrPause == m_pImpl->m_bMainWavePlaying) { return; }
  m_pImpl->m_bMainWavePlaying = bPlayOrPause;
  std::printf("[AUDIOMANAGER]: Main Wave %s\n"
    , m_pImpl->m_bMainWavePlaying ? "PLAYING" : "PAUSING");
  if (m_pImpl->m_bMainWavePlaying) {
    PlayAudioStream(m_pImpl->m_streamMainWave);
  } else {
    PauseAudioStream(m_pImpl->m_streamMainWave);
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
bool stemcapsulax::AudioManager::isMainWavePlaying() const
{
  return m_pImpl->m_bMainWavePlaying;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
const stemcapsulax::AudioManager::StatusData&
  stemcapsulax::AudioManager::statusData() const
{
  return m_pImpl->m_data;
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::AudioManager::Impl::Impl()
: m_bMainWavePlaying{ false }
, m_bExitThread     { false }
, m_bThreadStarted  { false }
{
  m_data.uFTQueueSizeInSamples = 0;
  m_data.uNFTProcessedSamples  = 0;
  m_data.uNStreamedSamples     = 0;
  m_data.uNFTDequeuedSamples   = 0;
  m_data.uNFFTReadyResults     = 0;

  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(STEMCAPSULAX_AUDIOMANAGER_BUFSZ);
  
  m_threadFFT = std::thread([=]() {
    std::vector<f32> vinput;
    std::queue<f32> qstored;
    m_bThreadStarted = true;
    while (!m_bExitThread) {
      // preleva il buffer dalla coda
      { std::lock_guard<std::mutex> guard{ gASCS.mtxqdata };
        if (!gASCS.qdata.empty()) {
          const auto& ab = gASCS.qdata.front();
          size_t szUsedSamples = 0, szStoredSamples = 0;
          for (size_t j = 0;(j < qstored.size()) && (vinput.size() < STEMCAPSULAX_AUDIOMANAGER_FFTSZ);++j) {
            vinput.push_back(qstored.front());
            qstored.pop();
          }
          for (size_t j = 0;(j < ab.count) && (vinput.size() < STEMCAPSULAX_AUDIOMANAGER_FFTSZ);++j) {
            vinput.push_back(ab.itemAt(j));
            ++szUsedSamples;
          }
          for (size_t j = szUsedSamples;j < ab.count;++j) {
            qstored.push(ab.itemAt(j));
            ++szStoredSamples;
          }
          gASCS.qdata.pop();
          m_data.uNFTDequeuedSamples += (szUsedSamples + szStoredSamples);
        }
      }
      
      // nutri il processore FFT (attenzione, perché lavora su 4096 campioni
      // stereo, cioè 2048 campioni per canale)
      if (vinput.size() >= STEMCAPSULAX_AUDIOMANAGER_FFTSZ) {
        const size_t szNHALF = STEMCAPSULAX_AUDIOMANAGER_FFTSZ / 2;
        FFTResultDataPackage rdt; // notare il 2048 !
        rdt.res = fftsimd::spectrum_stereo_48k_float32(vinput.data(), szNHALF, true);
        STEMCAPSULAX_CAPTURE_CPU(rdt.res.magL.empty(), "Empty FFT L");
        STEMCAPSULAX_CAPTURE_CPU(rdt.res.magR.empty(), "Empty FFT R");
        m_ComputeEnergyForResult(rdt.res, rdt);
        { std::lock_guard<std::mutex> guard{ m_mtxqfftres };
          m_qfftres.push(rdt); }
        m_data.uNFTProcessedSamples += vinput.size();
        vinput.clear();
      }
    }
  });
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
stemcapsulax::AudioManager::Impl::~Impl()
{
  m_bExitThread = true;
  if (m_threadFFT.joinable()) { m_threadFFT.join(); }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * METHOD
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
void stemcapsulax::AudioManager::Impl::m_ComputeEnergyForResult(
    const fftsimd::SpectrumResult& in
  , FFTResultDataPackage& dataout)
{
  // Dati N campioni della FFT, la frequenza corrispondente al campione k si
  // calcola come fk = (k * fs) / N, dove fs è la frequenza di campionamento.
  // In generale, fs = 2 * fny dove fny è la frequenza di Nyquist. Questo
  // implica che la significatività fisica dei campioni FFT è dal campione
  // k=0 al campione k=fny, quindi i primi N/2 campioni. Questo si applica
  // a segnali reali, come quelli su cui lavora il nostro componente FFT.
  // Ricordiamo che per k=0 si ha la componente DC.

  const f32 fs = f32(STEMCAPSULAX_AUDIOMANAGER_FRQHZ);
  dataout.fL_Energy = .0f;
  dataout.pairFreqAmpMaxLft = 
    std::make_pair(.0f, -std::numeric_limits<f32>::max());
  dataout.pairFreqAmpMinLft = 
    std::make_pair(.0f,  std::numeric_limits<f32>::max());
  size_t N = in.magL.size();
  for (size_t k = 0;k < N / 2;++k) {
    auto value = in.magL.at(k);
    dataout.fL_Energy += value * value;
    if (value > dataout.pairFreqAmpMaxLft.second) {
      dataout.pairFreqAmpMaxLft = std::make_pair(f32(k) * fs * .5f / f32(N)
        , std::max(dataout.pairFreqAmpMaxLft.second, value));
    }
    if (value < dataout.pairFreqAmpMinLft.second) {
      dataout.pairFreqAmpMinLft = std::make_pair(f32(k) * fs * .5f / f32(N)
        , std::min(dataout.pairFreqAmpMaxLft.second, value));
    }
  }
  dataout.fR_Energy = .0f;
  dataout.pairFreqAmpMaxRgt = 
    std::make_pair(.0f, -std::numeric_limits<f32>::max());
  dataout.pairFreqAmpMinRgt = 
    std::make_pair(.0f,  std::numeric_limits<f32>::max());
  N = in.magR.size();    
  for (size_t k = 0;k < N / 2;++k) {
    auto value = in.magR.at(k);
    dataout.fR_Energy += value * value;
    if (value > dataout.pairFreqAmpMaxRgt.second) {
      dataout.pairFreqAmpMaxRgt = std::make_pair(f32(k) * fs * .5f / f32(N)
        , std::max(dataout.pairFreqAmpMaxRgt.second, value));
    }
    if (value < dataout.pairFreqAmpMinRgt.second) {
      dataout.pairFreqAmpMinRgt = std::make_pair(f32(k) * fs * .5f / f32(N)
        , std::min(dataout.pairFreqAmpMaxRgt.second, value));
    }
  }
}

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 * LOCAL FUNCTIONS (Questa callback gira in un altro thread RAYLIB)
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
static void AudioStreamCallback48x2(void* out,uint32_t count)
{
  if (gASCS.iAudioWindowCursor >= gASCS.iFramesCount) {
    return;
  }

  f32* pCurAudioData = gASCS.pAudioSamples + gASCS.iAudioWindowCursor;
  u32 nstereo = count * 2;
  audiosamplebuffer48x2 asb;
  for (u32 k = 0;k < nstereo;k += 2) {
    reinterpret_cast<float*>(out)[k  ] = *(pCurAudioData + (k  ));
    reinterpret_cast<float*>(out)[k+1] = *(pCurAudioData + (k+1));
    asb.data[k  ] = *(pCurAudioData + (k  ));
    asb.data[k+1] = *(pCurAudioData + (k+1));
    asb.count += 2;
  }
  gASCS.iAudioWindowCursor += nstereo;
  { std::lock_guard<std::mutex> g{ gASCS.mtxqdata };
    gASCS.qdata.push(asb); }
}
