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
  ~RtSoundProvider() = default;

  void addClient(std::weak_ptr<RtSoundClient> client_);

  void checkClients();
  void orderClients();
  void notifyUpdateSoundDevices();
  void notifyConfigureStream();
  void notifyApplyStreamConfig();
  void notifyStreamDataReady();

  void setStreamDevices(const std::vector<RtAudio::DeviceInfo> &devices) {
    _streamDevices = devices;
  }
  const std::vector<RtAudio::DeviceInfo> &streamDevcies() const {
    return _streamDevices;
  }

  RtSoundSetup &streamSetup() {
    const auto ptr{_streamSetup.get()};
    assert(ptr != nullptr);
    return (*ptr);
  }

  RtSoundData &streamData() {
    const auto ptr{_streamData.get()};
    assert(ptr != nullptr);
    return (*ptr);
  }

private:
  std::vector<std::weak_ptr<RtSoundClient>> _clients;
  std::shared_ptr<RtSoundSetup> _streamSetup{new RtSoundSetup()};
  std::shared_ptr<RtSoundData> _streamData{new RtSoundData()};
  std::vector<RtAudio::DeviceInfo> _streamDevices;
};
