#pragma once
#include "RtSoundBaseGen.h"
#include <random>

namespace RtSound {
class NoiseGen : public BaseGen
{
public:
  NoiseGen() { setClientName("Noise Generator"); }
  ~NoiseGen() override = default;

  virtual const std::type_info &clientTypeId() const override {
    return typeid(this);
  }

private:
  void generate(float *buffer, int nFrames) override final {
    const float A{amplitudeNormal() / 3.f};
    for (int i = 0; i != nFrames; ++i) {
      std::normal_distribution<float> val(0.0f, A);
      buffer[i] = val(_rgen);
    }
  }

  std::atomic_int _amplitude{1};
  std::random_device _rdev;
  std::mt19937 _rgen{_rdev()};
};
} // namespace RtSound
