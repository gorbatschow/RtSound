#pragma once
#include "RtSoundInfo.h"
#include "RtSoundProvider.h"
#include <RtAudio.h>
#include <algorithm>
#include <atomic>
#include <memory>
#include <vector>

namespace RtSound {
class IO
{
public:
  IO() = default;
  ~IO() = default;

  void startSoundEngine(RtAudio::Api api = RtAudio::UNSPECIFIED);
  void setupSoundStream();
  void startSoundStream(bool shot = false);
  void stopSoundStream();

  inline void addClient(std::weak_ptr<Client> client) {
    _streamProvider->addClient(client);
  }
  inline void checkClients() { _streamProvider->checkClients(); }
  inline void orderClients() { _streamProvider->orderClients(); }

  inline const Info &streamInfo() const { return _streamInfo; }
  inline Provider &streamProvider() { return (*_streamProvider); }
  inline StreamSetup &streamSetup() { return (_streamProvider->streamSetup()); }

private:
  std::shared_ptr<RtAudio> _rta;
  std::shared_ptr<Provider> _streamProvider{new Provider()};
  Info _streamInfo;

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
} // namespace RtSound
