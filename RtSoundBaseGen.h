#pragma once
#include "RtSoundClient.h"
#include <atomic>

class RtSoundBaseGen : public RtSoundClient {
public:
  RtSoundBaseGen(int priority = 0);

  inline void setInputEnabled(bool value) { _inputEnabled.exchange(value); }
  inline bool inputEnabled() const { return _inputEnabled.load(); }

  inline void setInputChannel(int value) { _inputChannel.exchange(value); }
  inline int inputChannel() const { return _inputChannel.load(); }

  inline void setOutputEnabled(bool value) { _outputEnabled.exchange(value); }
  inline bool outputEnabled() const { return _outputEnabled.load(); }

  inline void setOutputChannel(int value) { _outputChannel.exchange(value); }
  inline int outputChannel() const { return _outputChannel.load(); }

protected:
  virtual void generate(float *buffer, int nFrames, double t) const {}

private:
  std::atomic_int _inputChannel{};
  std::atomic_bool _inputEnabled{false};

  std::atomic_int _outputChannel{};
  std::atomic_bool _outputEnabled{false};

  virtual void receiveStreamData(const RtStreamData &streamData) override final;

  void fillInput(const RtStreamData &streamData) const;
  void fillOutput(const RtStreamData &streamData) const;
};
