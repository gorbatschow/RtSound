#pragma once
#include "RtSoundData.h"
#include "RtSoundSetup.h"
#include <cassert>
#include <memory>

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
  virtual void streamDataReady(){};

  const RtSoundData &streamData() const {
    const auto ptr{_streamData.lock().get()};
    if (!ptr)
      throw std::bad_weak_ptr();
    return *ptr;
  }

private:
  int _priority{};
  std::weak_ptr<RtSoundData> _streamData{};
};
