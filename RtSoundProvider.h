#pragma once
#include "RtSoundSetup.h"
#include "RtSoundStream.h"
#include <memory>
#include <mutex>
#include <vector>
class RtSoundClient;

class RtSoundProvider {
public:
  RtSoundProvider();
  ~RtSoundProvider() {}

  void addClient(std::weak_ptr<RtSoundClient> client);

protected:
  void checkClients();
  void orderClients();
  void notifyUpdateSoundDevices(const std::vector<RtAudio::DeviceInfo> &);
  void notifyConfigureStream(RtSoundSetup &);
  void notifyApplyStreamConfig(const RtSoundSetup &);
  void notifyReceiveStreamData(const RtStreamData &);

private:
  std::vector<std::weak_ptr<RtSoundClient>> _clients;
};
