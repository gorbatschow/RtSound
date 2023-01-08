#pragma once
#include "RtSoundBaseGen.h"
#include <atomic>
#include <cmath>

class RtSoundToneGen : public RtSoundBaseGen {
public:
  RtSoundToneGen(int priority = 0) : RtSoundBaseGen(priority) {
    setClientName("Tone Generator");
  };
  ~RtSoundToneGen() override = default;

  virtual const std::type_info &clientTypeId() const override {
    return typeid(this);
  }

  inline void setFrequencyPercent(float percent) {
    assert(0.0f <= percent && percent <= 100.0f);
    _frequency_mHz.exchange(int(percentTo_mHz(_sampleRate, percent)));
  }

  inline void setFrequencyHertz(float hertz) {
    _frequency_mHz.exchange(int(hertz * 1e+3f));
  }

  inline float frequencyHertz() const {
    return float(_frequency_mHz.load()) * 1e-3f;
  }

  static inline float percentTo_Hz(float fs, float fp) {
    return fp * 0.005f * fs;
  }

  static inline float percentTo_mHz(float fs, float fp) {
    return fp * 0.005f * fs * 1e+3f;
  }

private:
  void applyStreamConfig(const RtSoundSetup &setup) override final {
    _sampleRate.exchange(setup.sampleRate());
  }

  void generate(float *buffer, int nFrames) override final {
    const double f{frequencyHertz()};
    const double A{amplitudeNormalized()};
    const double dt{1.0f / float(_sampleRate.load())};
    for (int i = 0; i != nFrames; ++i) {
      buffer[i] = float(A * cos(2 * M_PI * f * _t));
      _t += dt;
    }
  }

  std::atomic_int _sampleRate{};
  std::atomic_int _frequency_mHz{};

  double _t{};
};
