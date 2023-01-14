#pragma once
#include "RtSoundClient.h"
#include <atomic>

class RtSoundBaseGen : public RtSoundClient {
public:
  RtSoundBaseGen(int priority = 0) : RtSoundClient(priority) {}
  virtual ~RtSoundBaseGen() = default;

  virtual const std::type_info &clientTypeId() const override {
    return typeid(this);
  }

  // Enabled
  // ---------------------------------------------------------------------------
  inline void setEnabled(bool value) { _enabled.exchange(value); }
  inline bool enabled() const { return _enabled.load(); }

  // Input/Output
  // ---------------------------------------------------------------------------
  inline void setSendToInput(bool toInput) { _toInput.exchange(toInput); }
  inline bool sendToInput() { return _toInput.load(); }

  // Channel
  // ---------------------------------------------------------------------------
  inline void setChannel(int value) { _channel.exchange(value); }
  inline int inputChannel() const { return _channel.load(); }

  // Amplitude Normal
  // --------------------------------------------------------------------------
  inline void setAmplitudeNormal(float normal) {
    assert(0.0f <= normal && normal <= 1.0f);
    _amplitude_mNormal.exchange(int(normal * 1e+3f));
  }

  inline float amplitudeNormal() {
    return float(_amplitude_mNormal.load()) * 1e-3f;
  }

  // Amplitude Percent
  // --------------------------------------------------------------------------
  inline void setAmplitudePercent(float value) {
    assert(0.0f <= value && value <= 100.0f);
    setAmplitudeNormal(value * 1e-2f);
  }

  inline float amplitudePercent() { return amplitudeNormal() * 1e+2f; }

protected:
  virtual void streamDataReady(const RtSoundData &data) override {
    if (!_enabled.load()) {
      return;
    }
    if (_toInput.load()) {
      std::lock_guard lock(data.mutex);
      generate(data.inputBuffer(_channel), data.framesN());
    } else {
      std::lock_guard lock(data.mutex);
      generate(data.outputBuffer(_channel), data.framesN());
    }
  }

  virtual void generate(float *buffer, int nFrames) {}

private:
  std::atomic_bool _enabled{};
  std::atomic_bool _toInput{};
  std::atomic_int _channel{};
  std::atomic_int _amplitude_mNormal{};
};
