#pragma once
#include <RtAudio.h>
#include <memory>

namespace RtSound {
class Provider;
class Client;
class StreamSetup;
class StreamData;
class StreamInfo;

class IO {
public:
  IO();
  ~IO();

  void startSoundEngine(RtAudio::Api api);
  void setupSoundStream();
  void startSoundStream(bool shot = false);
  void stopSoundStream();

  void addClient(std::weak_ptr<Client> client);

private:
  std::shared_ptr<RtAudio> _rta;
  std::shared_ptr<Provider> _streamProvider;
  std::unique_ptr<StreamSetup> _streamSetup;
  std::unique_ptr<StreamData> _streamData;
  std::shared_ptr<StreamInfo> _streamInfo;
  std::vector<std::weak_ptr<Client>> _clients;

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
