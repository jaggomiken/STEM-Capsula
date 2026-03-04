// fft_stereo_simd.hpp
// Build notes:
//  - x86_64: compile with -msse4.2 (gcc/clang) or /arch:SSE2 + intrinsics (MSVC) and ensure SSE is enabled.
//  - ARM: compile with -mfpu=neon (32-bit) or default on AArch64 (NEON available).
// This code computes two real-input FFTs (L and R) by converting to complex and running complex FFT.
// For real signals you can optimize further (real FFT), but this is already a solid baseline.

#include <cstdint>
#include <cstring>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>

#if defined(__SSE4_2__) || (defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86)))
  #include <immintrin.h>
  #define FFT_USE_SSE 1
#else
  #define FFT_USE_SSE 0
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__)
  #include <arm_neon.h>
  #define FFT_USE_NEON 1
#else
  #define FFT_USE_NEON 0
#endif

namespace fftsimd {

// ------------------------ utilities ------------------------

static inline bool is_pow2(std::size_t n) { return n && ((n & (n - 1)) == 0); }

static inline std::size_t bit_reverse(std::size_t x, unsigned log2n) {
  std::size_t n = 0;
  for (unsigned i = 0; i < log2n; ++i) {
    n = (n << 1) | (x & 1);
    x >>= 1;
  }
  return n;
}

static inline unsigned ilog2_u(std::size_t n) {
  unsigned r = 0;
  while ((std::size_t(1) << r) < n) ++r;
  return r;
}

// Hann window (good default for spectrum display)
static inline void apply_hann_window(float* dst, const float* src, std::size_t N) {
  const float twoPiOver = 2.0f * float(M_PI) / float(N - 1);
  for (std::size_t i = 0; i < N; ++i) {
    float w = 0.5f - 0.5f * std::cos(twoPiOver * float(i));
    dst[i] = src[i] * w;
  }
}

// ------------------------ SIMD complex multiply for 2 complex numbers ------------------------
// We operate on packed floats: [re0, im0, re1, im1]

// (a+ib)*(c+id) = (ac - bd) + i(ad + bc)

#if FFT_USE_SSE
static inline __m128 cmul2_sse(__m128 a, __m128 w) {
  // a = [ar0, ai0, ar1, ai1]
  // w = [wr0, wi0, wr1, wi1] (usually same twiddle replicated, but we support both)
  __m128 arar_aiai = _mm_moveldup_ps(a);      // [ar0, ar0, ar1, ar1]
  __m128 aiai      = _mm_movehdup_ps(a);      // [ai0, ai0, ai1, ai1]
  __m128 wrwi_wrwi = w;                       // [wr0, wi0, wr1, wi1]
  __m128 wiw_r = _mm_shuffle_ps(w, w, _MM_SHUFFLE(2,3,0,1)); // [wi0, wr0, wi1, wr1]

  __m128 ac_ad = _mm_mul_ps(arar_aiai, wrwi_wrwi);
  __m128 bd_bc = _mm_mul_ps(aiai, wiw_r);

  // real = ac - bd ; imag = ad + bc
  // With the chosen shuffles, this becomes:
  // [ar*wr, ar*wi, ar*wr, ar*wi] and [ai*wi, ai*wr, ai*wi, ai*wr]
  // so: [ar*wr - ai*wi, ar*wi + ai*wr, ...]
  __m128 res = _mm_addsub_ps(ac_ad, bd_bc);
  return res;
}
#endif

#if FFT_USE_NEON
static inline float32x4_t cmul2_neon(float32x4_t a, float32x4_t w) {
  // a = [ar0, ai0, ar1, ai1]
  // w = [wr0, wi0, wr1, wi1]
  float32x4_t arar = vdupq_laneq_f32(a, 0); // will be [ar0, ar0, ar0, ar0]
  float32x4_t ai_ai = vdupq_laneq_f32(a, 1); // [ai0, ai0, ai0, ai0]
  // But we need per-lane ar0/ar1. Easier: use zip/uzp to build [ar0, ar0, ar1, ar1] etc.
  float32x4_t ar = vuzp1q_f32(a, a); // [ar0, ar1, ar0, ar1] (pattern)
  float32x4_t ai = vuzp2q_f32(a, a); // [ai0, ai1, ai0, ai1]

  // Make [ar0, ar0, ar1, ar1]
  float32x4_t arar2 = vzip1q_f32(ar, ar); // [ar0, ar0, ar1, ar1]
  float32x4_t aiai2 = vzip1q_f32(ai, ai); // [ai0, ai0, ai1, ai1]

  float32x4_t wrwi = w; // [wr0, wi0, wr1, wi1]
  float32x4_t wiw_r = vrev64q_f32(w); // swap within pairs: [wi0, wr0, wi1, wr1]

  float32x4_t ac_ad = vmulq_f32(arar2, wrwi);
  float32x4_t bd_bc = vmulq_f32(aiai2, wiw_r);

  // Need [ac - bd, ad + bc, ...]. We can do it with lane-wise sign flip and add.
  // bd_bc = [ai*wi, ai*wr, ai*wi, ai*wr]
  // real: ac - bd => ac_ad lane0 - bd_bc lane0 ; imag: ac_ad lane1 + bd_bc lane1
  // Achieve via multiplying bd_bc by [ -1, +1, -1, +1 ] then add.
  const float32x4_t sign = { -1.f, +1.f, -1.f, +1.f };
  float32x4_t bd_bc_signed = vmulq_f32(bd_bc, sign);
  return vaddq_f32(ac_ad, bd_bc_signed);
}
#endif

// ------------------------ FFT core ------------------------
// In-place complex FFT (std::complex<float>) for length N (power of two).
// Twiddle factors are computed per stage (can be cached for repeated calls).

static inline void fft_inplace(std::complex<float>* a, std::size_t N) {
  const unsigned log2n = ilog2_u(N);

  // bit-reversal permutation
  for (std::size_t i = 0; i < N; ++i) {
    std::size_t j = bit_reverse(i, log2n);
    if (j > i) std::swap(a[i], a[j]);
  }

  // iterative stages
  for (std::size_t s = 1; s <= log2n; ++s) {
    const std::size_t m  = std::size_t(1) << s;       // FFT size of this stage
    const std::size_t m2 = m >> 1;                    // half
    const float theta = -2.0f * float(M_PI) / float(m);
    const float wpr = std::cos(theta);
    const float wpi = std::sin(theta);

    // For each block of size m
    for (std::size_t k = 0; k < N; k += m) {
      // w starts at 1 + i0
      float wr = 1.0f, wi = 0.0f;

      // butterflies
      std::size_t j = 0;

#if FFT_USE_SSE || FFT_USE_NEON
      // SIMD path: handle 2 butterflies at a time when possible (j and j+1), for each block.
      for (; j + 1 < m2; j += 2) {
        // twiddles for j and j+1
        float wr0 = wr;
        float wi0 = wi;
        // advance once to get wr1/wi1
        float wr1 = wr0 * wpr - wi0 * wpi;
        float wi1 = wr0 * wpi + wi0 * wpr;

        // Load upper values: a[k + j + m2], a[k + j+1 + m2]
        std::complex<float> u0 = a[k + j];
        std::complex<float> u1 = a[k + j + 1];

        std::complex<float> t0 = a[k + j + m2];
        std::complex<float> t1 = a[k + j + 1 + m2];

        // Pack t's as [re0, im0, re1, im1]
#if FFT_USE_SSE
        __m128 tv = _mm_set_ps(t1.imag(), t1.real(), t0.imag(), t0.real());
        __m128 wv = _mm_set_ps(wi1, wr1, wi0, wr0); // [wr0, wi0, wr1, wi1] but packed as set_ps is reversed
        // Fix: set as [re0, im0, re1, im1] => use set_ps(im1, re1, im0, re0)
        wv = _mm_set_ps(wi1, wr1, wi0, wr0);
        __m128 tw = cmul2_sse(tv, wv);
        // Extract tw to floats
        alignas(16) float tmp[4];
        _mm_store_ps(tmp, tw); // tmp = [re0, im0, re1, im1]
        std::complex<float> v0(tmp[0], tmp[1]);
        std::complex<float> v1(tmp[2], tmp[3]);
#elif FFT_USE_NEON
        float32x4_t tv = { t0.real(), t0.imag(), t1.real(), t1.imag() };
        float32x4_t wv = { wr0, wi0, wr1, wi1 };
        float32x4_t tw = cmul2_neon(tv, wv);
        float tmp[4];
        vst1q_f32(tmp, tw);
        std::complex<float> v0(tmp[0], tmp[1]);
        std::complex<float> v1(tmp[2], tmp[3]);
#endif

        a[k + j]         = u0 + v0;
        a[k + j + m2]    = u0 - v0;
        a[k + j + 1]     = u1 + v1;
        a[k + j + 1 + m2]= u1 - v1;

        // advance w by 2 steps
        // After one step: (wr1, wi1) is already computed.
        // Next step from (wr1,wi1):
        float wr2 = wr1 * wpr - wi1 * wpi;
        float wi2 = wr1 * wpi + wi1 * wpr;
        wr = wr2; wi = wi2;
      }
#endif

      // scalar remainder
      for (; j < m2; ++j) {
        std::complex<float> u = a[k + j];
        std::complex<float> t = a[k + j + m2];
        std::complex<float> w(wr, wi);
        std::complex<float> v = t * w;
        a[k + j]      = u + v;
        a[k + j + m2] = u - v;

        // update w: w *= w_m
        float nwr = wr * wpr - wi * wpi;
        float nwi = wr * wpi + wi * wpr;
        wr = nwr; wi = nwi;
      }
    }
  }
}

// ------------------------ Public API ------------------------
// Computes magnitude spectra for L and R.
// inputInterleaved: LRLRLR... length = 2*N floats (N frames)
// outMagL/outMagR: length N/2 + 1 each (DC..Nyquist)
// N must be power of two.

struct SpectrumResult {
  std::vector<float> magL;
  std::vector<float> magR;
};

static inline SpectrumResult spectrum_stereo_48k_float32(
    const float* inputInterleavedLR,
    std::size_t N,
    bool applyWindow = true)
{
  SpectrumResult out;
  if (!is_pow2(N) || N < 2) {
    return out; // empty
  }

  // Debug
#if 0  
  std::fprintf(stdout, "[SAMPLES IN: %5zu]: ", N);
  for (size_t k = 0;k < N;++k) { std::fprintf(stdout, "%f ", inputInterleavedLR[k]); }
  std::fprintf(stdout, "\n\n\n");
#endif  

  // Separate channels and (optional) window.
  std::vector<float> L(N), R(N);
  if (applyWindow) {
    std::vector<float> rawL(N), rawR(N);
    for (std::size_t i = 0; i < N; ++i) {
      rawL[i] = inputInterleavedLR[2*i + 0];
      rawR[i] = inputInterleavedLR[2*i + 1];
    }
    apply_hann_window(L.data(), rawL.data(), N);
    apply_hann_window(R.data(), rawR.data(), N);
  } else {
    for (std::size_t i = 0; i < N; ++i) {
      L[i] = inputInterleavedLR[2*i + 0];
      R[i] = inputInterleavedLR[2*i + 1];
    }
  }

  // Convert to complex buffers (imag=0) and run complex FFT for each channel.
  std::vector<std::complex<float>> cL(N), cR(N);
  for (std::size_t i = 0; i < N; ++i) {
    cL[i] = { L[i], 0.0f };
    cR[i] = { R[i], 0.0f };
  }

  fft_inplace(cL.data(), N);
  fft_inplace(cR.data(), N);

  // Magnitude (only 0..N/2 for real input)
  const std::size_t bins = N / 2 + 1;
  out.magL.resize(bins);
  out.magR.resize(bins);

  // Scale: for visualization you often want 2/N (except DC & Nyquist), but keep it simple here:
  const float invN = 1.0f / float(N);

  out.magL[0] = std::abs(cL[0]) * invN;
  out.magR[0] = std::abs(cR[0]) * invN;

  for (std::size_t k = 1; k + 1 < bins; ++k) {
    out.magL[k] = 2.0f * std::abs(cL[k]) * invN;
    out.magR[k] = 2.0f * std::abs(cR[k]) * invN;
  }

  out.magL[bins - 1] = std::abs(cL[bins - 1]) * invN; // Nyquist
  out.magR[bins - 1] = std::abs(cR[bins - 1]) * invN;

  return out;
}

} // namespace fftsimd
