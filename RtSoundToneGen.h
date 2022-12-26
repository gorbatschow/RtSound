#pragma once
#include "RtSoundClient.h"
#include <atomic>

class RtSoundToneGen : public RtSoundClient {
public:
  RtSoundToneGen(int priority = 0);
  ~RtSoundToneGen();

  inline void setAmplitudeIndex(int value) { _amplitudeIndex.exchange(value); }
  inline int amplitudeIndex() { return _amplitudeIndex.load(); }
  inline float amplitude() { return _amplitudeIndex.load() * _da; }

  inline void setFrequencyIndex(int value) { _frequencyIndex.exchange(value); }
  inline int frequencyIndex() const { return _frequencyIndex.load(); }
  inline float frequency() const { return _frequencyIndex.load() * _df; }

  inline void setInputEnbaled(bool value) { _inputEnabled.exchange(value); }
  inline bool inputEnabled() const { return _inputEnabled; }

  inline void setOutputEnabled(bool value) { _outputEnabled.exchange(value); }
  inline bool outputEnabled() const { return _outputEnabled; }

  inline void setInputChannel(int value) { _inputChannel.exchange(value); }
  inline int inputChannel() const { return _inputChannel.load(); }

  inline void setOutputChannel(int value) { _outputChannel.exchange(value); }
  inline int outputChannel() const { return _outputChannel.load(); }

protected:
  virtual void beforeStartStream(const RtSoundSetup &setup) override;
  virtual void receiveStreamData(const RtStreamData &streamData) override;

private:
  float _dt{};
  float _df{};
  float _da{1.0f / 100.0f};
  std::atomic_int _amplitudeIndex{};
  std::atomic_int _frequencyIndex{};
  std::atomic_int _inputChannel{};
  std::atomic_int _outputChannel{};
  std::atomic_bool _inputEnabled{false};
  std::atomic_bool _outputEnabled{false};

  void generateTone(float *buffer, int nFrames, double t);
};
