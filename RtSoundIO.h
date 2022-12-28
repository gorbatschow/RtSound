#pragma once
#include "RtSoundProvider.h"
#include "RtSoundSetup.h"
#include "RtSoundStream.h"
#include <RtAudio.h>
#include <algorithm>
#include <atomic>
#include <memory>
#include <vector>

class RtSoundIO : public RtSoundProvider {
public:
  RtSoundIO();

  void startSoundEngine(RtAudio::Api api = RtAudio::UNSPECIFIED);
  void startSoundStream(bool shot = false);
  void stopSoundStream();
  void applySoundSetup();

  inline const RtSoundSetup &currSoundSetup() const { return _currSetupPub; }
  inline RtSoundSetup &nextSoundSetup() { return _nextSetup; }
  inline RtAudio::DeviceInfo currInputDevice() const {
    return _rta->getDeviceInfo(_currSetupPub.inputStream().deviceId);
  }
  inline RtAudio::DeviceInfo currOutputDevice() const {
    return _rta->getDeviceInfo(_currSetupPub.outputStream().deviceId);
  }
  inline RtStreamInfo streamInfo() const { return _streamInfo.load(); }

  std::vector<RtAudio::DeviceInfo> listSoundDevices() const;

private:
  std::unique_ptr<RtAudio> _rta;
  RtSoundSetup _currSetup, _currSetupPub, _nextSetup;
  RtStreamData _streamData{this};
  std::atomic<RtStreamInfo> _streamInfo;

  static int onHandleStream(void *outputBuffer, void *inputBuffer,
                            unsigned int nFrames, double streamTime,
                            RtAudioStreamStatus status, void *streamDataPtr);
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
