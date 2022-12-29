#pragma once
#include "RtSoundData.h"
#include "RtSoundSetup.h"

class RtSoundClient {
  friend class RtSoundProvider;

public:
  RtSoundClient(int priority = 0) : _priority(priority) {}
  virtual ~RtSoundClient() {}

  inline void setPriority(int priority) { _priority = priority; }
  inline int priority() const { return _priority; };

protected:
  virtual void updateSoundDevices(const std::vector<RtAudio::DeviceInfo> &) {}
  virtual void configureStream(RtSoundSetup &) {}
  virtual void applyStreamConfig(const RtSoundSetup &) {}
  virtual void receiveStreamData(const RtSoundData &) {}

private:
  int _priority{};
};
