#pragma once
#include "RtSoundBaseGen.h"
#include "RtSoundFrequencyUnit.h"
#include <atomic>
#include <cmath>

namespace RtSound {
class ToneGen : public BaseGen
{
public:
  // Constructor
  ToneGen(int priority = 0)
      : BaseGen(priority) {
    setClientName("Tone Generator");
  };

  // Destructor
  ~ToneGen() override = default;

  // Client Type Id
  virtual const std::type_info &clientTypeId() const override {
    return typeid(this);
  }

  // Frequency Index
  // ---------------------------------------------------------------------------
  inline void setFrequencyIndex(float index) {
    _frequencyUnit.exchange(RtSound::Frequency_Index);
    _frequencyValue.exchange(index);
  }

  inline float frequencyIndex() const {
    return frequency(RtSound::Frequency_Index);
  }

  // Frequency Hertz
  // ---------------------------------------------------------------------------
  inline void setFrequencyHertz(float hertz) {
    _frequencyUnit.exchange(RtSound::Frequency_mHertz);
    _frequencyValue.exchange(int(RtSound::hertz_to_mHertz(hertz)));
  }

  inline float frequencyHertz() const {
    return frequency(RtSound::Frequency_Hertz);
  }

  // Frequency Normal
  // ---------------------------------------------------------------------------
  inline void setFrequencyNormal(float normal) {
    using namespace RtSound;
    assert(0.0f <= normal && normal <= 1.0f);
    _frequencyUnit.exchange(Frequency_mNormal);
    _frequencyValue.exchange(int(normal_to_mNormal(normal)));
  }

  inline float frequencyNormal() const {
    return frequency(RtSound::Frequency_Normal);
  }

  // Frequency Percent
  // ---------------------------------------------------------------------------
  inline void setFrequencyPercent(float percent) {
    assert(0.0f <= percent && percent <= 100.0f);
    setFrequencyNormal(percent * 1e-2f);
  }

  inline float frequencyPercent() const { return frequencyNormal() * 1e+2f; }

  // Frequency Converter
  // ---------------------------------------------------------------------------
  inline float frequency(RtSound::FrequencyUnit toUnit) const {
    const auto fromUnit{RtSound::FrequencyUnit(_frequencyUnit.load())};
    const auto value{float(_frequencyValue.load())};
    const auto sampleRate{float(_sampleRate.load())};
    const auto bufferSize{float(_bufferSize.load())};

    return RtSound::convertFrequency(fromUnit, toUnit, value, sampleRate,
                                     bufferSize);
  }

private:
  void applyStreamConfig(const StreamSetup &setup) override final {
    _sampleRate.exchange(setup.sampleRate());
    _bufferSize.exchange(setup.bufferFrames());
  }

  void generate(float *buffer, int nFrames) override final {
    const float f{frequencyHertz()};
    const float A{amplitudeNormal()};
    const float dt{1.0f / float(_sampleRate.load())};
    for (int i = 0; i != nFrames; ++i) {
      buffer[i] = float(A * cos(2 * M_PI * f * _t));
      _t += dt;
    }
    _t = gateEnabled() ? 0.0f : _t;
  }

  std::atomic_int _sampleRate{};
  std::atomic_int _bufferSize{};
  std::atomic_int _frequencyUnit{};
  std::atomic_int _frequencyValue{};

  double _t{};
};
} // namespace RtSound
