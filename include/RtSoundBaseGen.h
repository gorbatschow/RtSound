#pragma once
#include "RtSoundClient.h"
#include <atomic>

namespace RtSound {
class BaseGen : public Client
{
public:
  // Constructor
  BaseGen(int priority = 0)
      : Client(priority) {}

  // Destructor
  virtual ~BaseGen() = default;

  // Client Type Id
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

  // Gate Enabled
  // ---------------------------------------------------------------------------
  inline void setGateEnabled(bool enabled) { _gateEnabled.exchange(enabled); }
  inline bool gateEnabled() const { return _gateEnabled.load(); }

  // Gate Interval
  // ---------------------------------------------------------------------------
  inline void setGateInterval(int interval) {
    _gateInterval.exchange(interval);
    _gateIntervalCounter.exchange(0);
  }
  inline int gateInterval() const { return _gateInterval.load(); }

  // Gate Open Frame
  // ---------------------------------------------------------------------------
  inline void setGateOpenFrame(int index) { _gateOpenFrame.exchange(index); }
  inline int gateOpenFrame() const { return _gateOpenFrame.load(); }

  // Gate Frame Count
  // ---------------------------------------------------------------------------
  inline void setGateFrameCount(int count) { _gateFrameCount.exchange(count); }
  inline int gateFrameCount() const { return _gateFrameCount.load(); }

  // Amplitude Normal
  // ---------------------------------------------------------------------------
  inline void setAmplitudeNormal(float normal) {
    assert(0.0f <= normal && normal <= 1.0f);
    _amplitude_mNormal.exchange(int(normal * 1e+3f));
  }

  inline float amplitudeNormal() {
    return float(_amplitude_mNormal.load()) * 1e-3f;
  }

  // Amplitude Percent
  // ---------------------------------------------------------------------------
  inline void setAmplitudePercent(float value) {
    assert(0.0f <= value && value <= 100.0f);
    setAmplitudeNormal(value * 1e-2f);
  }

  inline float amplitudePercent() { return amplitudeNormal() * 1e+2f; }

protected:
  virtual void applyStreamConfig(const StreamSetup &config) override {
    _gateIntervalCounter = 0;
  }

  virtual void streamDataReady(const Data &data) override {
    if (!_enabled.load()) {
      return;
    }

    const auto toInput{_toInput.load()};
    const auto channel{_channel.load()};

    if (!data.hasChannel(toInput, channel)) {
      return;
    }

    const auto isGated{_gateEnabled.load()};
    int bias = 0;
    int nFrames = data.framesN();

    if (isGated) {
      const auto interval{_gateInterval.load()};
      if (interval == 0) {
        return;
      }
      if (++_gateIntervalCounter < interval) {
        return;
      } else {
        _gateIntervalCounter.exchange(0);
      }
      const auto openFrame{_gateOpenFrame.load()};
      const auto frameCount{_gateFrameCount.load()};
      bias = std::clamp(openFrame, 0, nFrames);
      nFrames = std::clamp(frameCount, 0, nFrames - bias);
    }

    auto buffer{toInput ? data.inputBuffer(channel, bias)
                        : data.outputBuffer(channel, bias)};

    generate(buffer, nFrames);
  }

  virtual void generate(float *buffer, int nFrames) {}

private:
  std::atomic_bool _enabled{};
  std::atomic_bool _toInput{};
  std::atomic_int _channel{};
  std::atomic_bool _gateEnabled{};
  std::atomic_int _gateInterval{};
  std::atomic_int _gateOpenFrame{};
  std::atomic_int _gateFrameCount{};
  std::atomic_int _amplitude_mNormal{};
  std::atomic_int _gateIntervalCounter{};
};
} // namespace RtSound
