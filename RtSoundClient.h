#pragma once
#include "RtSoundSetup.h"
#include "RtSoundStream.h"

class RtSoundClient {
public:
  RtSoundClient(int priority = 0) : _priority(priority) {}
  virtual ~RtSoundClient() {}

  inline void setPriority(int priority) { _priority = priority; }
  inline int priority() const { return _priority; };

protected:
  friend class RtSoundProvider;
  virtual void configureStream(RtSoundSetup &) {}
  virtual void beforeStartStream(const RtSoundSetup &) {}
  virtual void receiveStreamData(const RtStreamData &) {}

private:
  int _priority{};
};
