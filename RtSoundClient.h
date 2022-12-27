#pragma once
#include "RtSoundSetup.h"
#include "RtSoundStream.h"

class RtSoundClient {
  friend class RtSoundProvider;

public:
  RtSoundClient(int priority = 0) : _priority(priority) {}
  virtual ~RtSoundClient() {}

  inline void setPriority(int priority) { _priority = priority; }
  inline int priority() const { return _priority; };

protected:
  virtual void configureStream(RtSoundSetup &) {}
  virtual void applyStreamConfig(const RtSoundSetup &) {}
  virtual void receiveStreamData(const RtStreamData &) {}

private:
  int _priority{};
};
