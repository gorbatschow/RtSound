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

  void addClient(const std::shared_ptr<Client> &client);
  void removeClient(const std::shared_ptr<Client> &client);

  mutable std::mutex ioMutex;

private:
  void initRta(RtAudio::Api api);
  void checkClients();
  void setupClient(const std::shared_ptr<Client> &client);
  void setupClients();
  void orderClients();

  void startSoundStreamRta();
  void startSoundStreamVirtual();
  void stopSoundStreamRta();
  void stopSoundStreamVirtual();

  void notifyClientPriorityChanged() const;
  void notifyUpdateSoundClients() const;
  void notifyUpdateSoundDevices() const;
  void notifyConfigureStream() const;
  void notifyApplyStreamConfig() const;
  void notifyStreamDataReady() const;

  std::shared_ptr<RtAudio> _rta;
  std::shared_ptr<StreamSetup> _streamSetup;
  std::shared_ptr<StreamData> _streamData;
  std::shared_ptr<StreamInfo> _streamInfo;
  std::vector<std::shared_ptr<Client> > _clients;

  struct VirtualStreamArgs;
  std::unique_ptr<VirtualStreamArgs> _virtualStreamArgs;

  static int onHandleStream(void *outputBuffer,
                            void *inputBuffer,
                            unsigned int nFrames,
                            double streamTime,
                            RtAudioStreamStatus status,
                            void *ioPtr);
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
