#pragma once
#include "RtSoundClient.h"
#include <atomic>

class RtSoundToneGen : public RtSoundClient {
public:
  RtSoundToneGen();

protected:
  virtual void beforeStartStream(const RtSoundSetup &setup) override;
  virtual void receiveStreamData(const RtStreamData &streamData) override;

private:
  float _dt{};
  float _df{};
  std::atomic_int _inputChannel{0};
  std::atomic_int _outputChannel{0};
  std::atomic_bool _inputEnabled{true};
  std::atomic_bool _outputEnabled{false};
};
