#pragma once
#include "RtSoundBaseGen.h"
#include <random>

class RtSoundNoiseGen : public RtSoundBaseGen {
public:
  RtSoundNoiseGen(int priority = 0) : RtSoundBaseGen(priority) {
    setClientName("Noise Generator");
  }
  ~RtSoundNoiseGen() override = default;

  virtual const std::type_info &clientTypeId() const override {
    return typeid(this);
  }

  inline void setAmplitude(int percent) { _amplitude.exchange(percent); }
  inline int amplitude() { return _amplitude.load(); }

private:
  void generate(float *buffer, int nFrames) override final {
    const float A{float(_amplitude.load()) * _da};
    for (int i = 0; i != nFrames; ++i) {
      std::normal_distribution<float> val(0.0f, A);
      buffer[i] = val(_rgen);
    }
  }

  float _da{1.0f / 100.0f / 3.3f};
  std::atomic_int _amplitude{1};
  std::random_device _rdev;
  std::mt19937 _rgen{_rdev()};
};
