#pragma once
#include "RtSoundStreamData.h"
#include "RtSoundStreamSetup.h"
#include <memory>
#include <mutex>
#include <vector>

namespace RtSound {
class Client;
class Provider : public std::enable_shared_from_this<Provider>
{
public:
  Provider() = default;
  ~Provider() = default;

  // Mutex
  mutable std::mutex providerMutex;

  inline void setRtAduio(std::weak_ptr<RtAudio> rta) {
    assert(rta.lock() != nullptr);
    _streamSetup->setRtAduio(rta);
  }

  void addClient(std::weak_ptr<Client> client_);
  void setSetupToData();
  void checkClients();
  void orderClients();
  void notifyUpdateSoundDevices();
  void notifyConfigureStream();
  void notifyApplyStreamConfig();
  void notifyStreamDataReady();

  inline StreamSetup &streamSetup() {
    const auto ptr{_streamSetup.get()};
    assert(ptr != nullptr);
    return (*ptr);
  }

  inline StreamData &streamData() {
    const auto ptr{_streamData.get()};
    assert(ptr != nullptr);
    return (*ptr);
  }

  inline long streamDataReadyTime() const { return _streamDataReadyTime; }

  inline const std::vector<std::weak_ptr<Client> > &clients() const {
    return _clients;
  }

private:
  std::vector<std::weak_ptr<Client> > _clients;
  std::shared_ptr<StreamSetup> _streamSetup{new StreamSetup()};
  std::shared_ptr<StreamData> _streamData{new StreamData()};
  long _streamDataReadyTime{};
};
} // namespace RtSound
