#pragma once
#include "RtSoundStreamData.h"
#include "RtSoundStreamSetup.h"
#include <RtAudio.h>
#include <memory>
#include <vector>

namespace RtSound {
class Client;
class StreamInfo;

class Provider {
public:
  Provider(const std::vector<std::weak_ptr<Client>> &list);
  ~Provider();

  void checkClients();
  void orderClients();

  void
  notifyUpdateSoundClients(const std::vector<std::weak_ptr<Client>> &clients);
  void notifyUpdateSoundDevices(const std::vector<RtAudio::DeviceInfo> &list);
  void notifyConfigureStream(StreamSetup &setup);
  void notifyApplyStreamConfig(const StreamSetup &setup);
  void notifyStreamDataReady(const StreamData &data);

  inline long streamDataReadyTime() const { return _streamDataReadyTime; }

  const std::vector<std::weak_ptr<Client>> &clients() const { return _clients; }

private:
  std::vector<std::weak_ptr<Client>> _clients;
  long _streamDataReadyTime{};
};
} // namespace RtSound
