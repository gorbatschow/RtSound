#pragma once
#include "RtSoundData.h"
#include "RtSoundSetup.h"
#include <memory>
#include <mutex>
#include <vector>
class RtSoundClient;

class RtSoundProvider {
public:
  RtSoundProvider();
  ~RtSoundProvider() {}

  void addClient(std::weak_ptr<RtSoundClient> client_);

protected:
  void checkClients();
  void orderClients();
  void notifyUpdateSoundDevices(const std::vector<RtAudio::DeviceInfo> &);
  void notifyConfigureStream(RtSoundSetup &);
  void notifyApplyStreamConfig(const RtSoundSetup &);
  void notifyStreamDataReady();

  std::vector<std::weak_ptr<RtSoundClient>> _clients;
  std::shared_ptr<RtSoundSetup> _streamSetup{new RtSoundSetup()};
  std::shared_ptr<RtSoundData> _streamData{new RtSoundData()};
};
