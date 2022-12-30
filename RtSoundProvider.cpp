#include "RtSoundProvider.h"
#include "RtSoundClient.h"
#include <algorithm>

RtSoundProvider::RtSoundProvider() {}

void RtSoundProvider::addClient(std::weak_ptr<RtSoundClient> client_) {
  const auto client{client_.lock().get()};
  assert(client != nullptr);

  client->setStreamSetup(_streamSetup);
  client->setStreamData(_streamData);
  _clients.push_back(client_);
}

void RtSoundProvider::checkClients() {
  std::remove_if(
      _clients.begin(), _clients.end(),
      [](const std::weak_ptr<RtSoundClient> &ptr) { return ptr.expired(); });
}

void RtSoundProvider::orderClients() {
  std::sort(_clients.begin(), _clients.end(),
            [](const std::weak_ptr<RtSoundClient> &first,
               const std::weak_ptr<RtSoundClient> &second) {
              return first.lock().get()->priority() < second.lock()->priority();
            });
}

void RtSoundProvider::notifyUpdateSoundDevices() {
  for (auto &client : _clients) {
    client.lock()->updateSoundDevices(_streamDevices);
  }
}

void RtSoundProvider::notifyConfigureStream() {
  for (auto &client : _clients) {
    client.lock()->configureStream(streamSetup());
  }
}

void RtSoundProvider::notifyApplyStreamConfig() {
  for (auto &client : _clients) {
    client.lock()->applyStreamConfig(streamSetup());
  }
}

void RtSoundProvider::notifyStreamDataReady() {
  for (auto &client : _clients) {
    client.lock()->streamDataReady();
  }
}
