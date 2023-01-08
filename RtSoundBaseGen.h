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

  // Amplitude
  // --------------------------------------------------------------------------
  inline void setAmplitudePercent(float percent) {
    assert(0.0f <= percent && percent <= 100.0f);
    _amplitude_mP.exchange(int(percent * 1e+3f));
  }
  inline float amplitudePercent() {
    return float(_amplitude_mP.load()) * 1e-3f;
  }
  inline float amplitudeNormalized() { return amplitudePercent() * 1e-2f; }

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
  std::atomic_int _amplitude_mP{};
};
