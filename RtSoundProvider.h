#pragma once
#include "RtSoundData.h"
#include "RtSoundSetup.h"
#include <memory>
#include <mutex>
#include <vector>
class RtSoundClient;

class RtSoundProvider : public std::enable_shared_from_this<RtSoundProvider> {
public:
  RtSoundProvider() = default;
  ~RtSoundProvider() = default;

  // Mutex
  mutable std::mutex mutex;

  inline void setRtAduio(std::weak_ptr<RtAudio> rta) {
    assert(rta.lock() != nullptr);
    _streamSetup->setRtAduio(rta);
  }

  void addClient(std::weak_ptr<RtSoundClient> client_);
  void setSetupToData();
  void checkClients();
  void orderClients();
  void notifyUpdateSoundDevices();
  void notifyConfigureStream();
  void notifyApplyStreamConfig();
  void notifyStreamDataReady();

  inline RtSoundSetup &streamSetup() {
    const auto ptr{_streamSetup.get()};
    assert(ptr != nullptr);
    return (*ptr);
  }

  inline RtSoundData &streamData() {
    const auto ptr{_streamData.get()};
    assert(ptr != nullptr);
    return (*ptr);
  }

  inline long streamDataReadyTime() const { return _streamDataReadyTime; }

  inline const std::vector<std::weak_ptr<RtSoundClient>> &clients() const {
    return _clients;
  }

private:
  std::vector<std::weak_ptr<RtSoundClient>> _clients;
  std::shared_ptr<RtSoundSetup> _streamSetup{new RtSoundSetup()};
  std::shared_ptr<RtSoundData> _streamData{new RtSoundData()};
  long _streamDataReadyTime{};
};
