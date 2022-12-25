#pragma once
#include "RtSoundProvider.h"
#include "RtSoundSetup.h"
#include "RtSoundStream.h"
#include <RtAudio.h>
#include <algorithm>
#include <atomic>
#include <memory>
#include <vector>
class RtSoundClient;

class RtSoundIO : public RtSoundProvider {
public:
  RtSoundIO();

  void startAudioEngine(RtAudio::Api api = RtAudio::UNSPECIFIED);
  void startAudioStream(bool shot = false);
  void stopAudioStream();

  inline const RtSoundSetup &currSoundSetup() const { return _currSetupPub; }
  inline RtSoundSetup &nextSoundSetup() { return _nextSetup; }
  inline RtAudio::DeviceInfo currInputDevice() const {
    return _rta->getDeviceInfo(_currSetupPub.inputStream().deviceId);
  }
  inline RtAudio::DeviceInfo currOutputDevice() const {
    return _rta->getDeviceInfo(_currSetupPub.outputStream().deviceId);
  }
  inline RtStreamInfo streamInfo() const { return _streamInfo.load(); }

private:
  std::unique_ptr<RtAudio> _rta;
  RtSoundSetup _currSetup, _currSetupPub, _nextSetup;
  RtStreamData _streamData{this};
  std::atomic<RtStreamInfo> _streamInfo;

  static int onHandleStream(void *outputBuffer, void *inputBuffer,
                            unsigned int nFrames, double streamTime,
                            RtAudioStreamStatus status, void *streamDataPtr);
};
