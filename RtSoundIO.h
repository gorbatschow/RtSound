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
  RtSoundIO();

  void startSoundEngine(RtAudio::Api api = RtAudio::UNSPECIFIED);
  void startSoundStream(bool shot = false);
  void stopSoundStream();

  inline void addClient(std::weak_ptr<RtSoundClient> client) {
    _streamProvider.addClient(client);
  }

  inline const RtSoundInfo &streamInfo() const { return _streamInfo; }
  inline RtSoundProvider &streamProvider() { return _streamProvider; }

private:
  std::shared_ptr<RtAudio> _rta;
  RtSoundInfo _streamInfo;
  RtSoundProvider _streamProvider;

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
