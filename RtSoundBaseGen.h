#pragma once
#include "RtSoundClient.h"
#include <atomic>

class RtSoundBaseGen : public RtSoundClient {
public:
  RtSoundBaseGen(int priority = 0);
  ~RtSoundBaseGen() = default;

  // Input
  // ---------------------------------------------------------------------------
  inline void setInputEnabled(bool value) {
    _inputChanged.exchange(true);
    _inputEnabled.exchange(value);
  }
  inline bool inputEnabled() const { return _inputEnabled.load(); }

  inline void setInputChannel(int value) {
    _inputChanged.exchange(true);
    _inputChannel.exchange(value);
  }
  inline int inputChannel() const { return _inputChannel.load(); }

  // Output
  // ---------------------------------------------------------------------------
  inline void setOutputEnabled(bool value) {
    _outputChanged.exchange(true);
    _outputEnabled.exchange(value);
  }
  inline bool outputEnabled() const { return _outputEnabled.load(); }

  inline void setOutputChannel(int value) {
    _outputChanged.exchange(true);
    _outputChannel.exchange(value);
  }
  inline int outputChannel() const { return _outputChannel.load(); }

protected:
  virtual void generate(float *buffer, int nFrames, double t) const {}

private:
  std::atomic_int _inputChannel{};
  std::atomic_bool _inputEnabled{};
  std::atomic_bool _inputChanged{};

  std::atomic_int _outputChannel{};
  std::atomic_bool _outputEnabled{};
  std::atomic_bool _outputChanged{};

  virtual void streamDataReady(const RtSoundData &data) override final;

  void fillSignal(const RtSoundData &streamData, float *buffer) const;
};
