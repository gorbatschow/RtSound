#pragma once
#include "RtSoundInfo.h"
#include "RtSoundProvider.h"
#include <RtAudio.h>
#include <algorithm>
#include <atomic>
#include <memory>
#include <vector>

class RtSoundIO {
public:
  RtSoundIO() = default;
  ~RtSoundIO() = default;

  void startSoundEngine(RtAudio::Api api = RtAudio::UNSPECIFIED);
  void setupSoundStream();
  void startSoundStream(bool shot = false);
  void stopSoundStream();

  inline void addClient(std::weak_ptr<RtSoundClient> client) {
    _streamProvider->addClient(client);
  }
  inline void checkClients() { _streamProvider->checkClients(); }
  inline void orderClients() { _streamProvider->orderClients(); }

  inline const RtSoundInfo &streamInfo() const { return _streamInfo; }
  inline RtSoundProvider &streamProvider() { return (*_streamProvider); }
  inline RtSoundSetup &streamSetup() {
    return (_streamProvider->streamSetup());
  }

private:
  std::shared_ptr<RtAudio> _rta;
  std::shared_ptr<RtSoundProvider> _streamProvider{new RtSoundProvider()};
  RtSoundInfo _streamInfo;

  static int onHandleStream(void *outputBuffer, void *inputBuffer,
                            unsigned int nFrames, double streamTime,
                            RtAudioStreamStatus status, void *ioPtr);
};

// -----------------------------------------------------------------------------
inline bool operator==(const RtAudio::DeviceInfo &devl,
                       const RtAudio::DeviceInfo &devr) {
  return devl.ID == devr.ID;
}

inline bool operator!=(const RtAudio::DeviceInfo &devl,
                       const RtAudio::DeviceInfo &devr) {
  return devl.ID != devr.ID;
}
