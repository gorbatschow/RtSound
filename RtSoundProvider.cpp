#include "RtSoundProvider.h"
#include "RtSoundClient.h"
#include <algorithm>

RtSoundProvider::RtSoundProvider() {}

void RtSoundProvider::addClient(std::weak_ptr<RtSoundClient> client) {
  if (client.expired()) {
    return;
  }
  client.lock()->_streamData = _streamData;
  _clients.push_back(client);
}

void RtSoundProvider::checkClients() {
  std::remove_if(
      _clients.begin(), _clients.end(),
      [](const std::weak_ptr<RtSoundClient> &ptr) { return ptr.expired(); });
}

void RtSoundProvider::orderClients() {
  checkClients();
  std::sort(_clients.begin(), _clients.end(),
            [](const std::weak_ptr<RtSoundClient> &first,
               const std::weak_ptr<RtSoundClient> &second) {
              return first.lock().get()->priority() < second.lock()->priority();
            });
}

void RtSoundProvider::notifyUpdateSoundDevices(
    const std::vector<RtAudio::DeviceInfo> &devices) {
  checkClients();
  for (auto &client : _clients) {
    client.lock()->updateSoundDevices(devices);
  }
}

void RtSoundProvider::notifyConfigureStream(RtSoundSetup &setup) {
  checkClients();
  for (auto &client : _clients) {
    client.lock()->configureStream(setup);
  }
}

void RtSoundProvider::notifyApplyStreamConfig(const RtSoundSetup &setup) {
  checkClients();
  for (auto &client : _clients) {
    client.lock()->applyStreamConfig(setup);
  }
}

void RtSoundProvider::notifyStreamDataReady() {
  checkClients();
  for (auto &client : _clients) {
    client.lock()->streamDataReady();
  }
}
