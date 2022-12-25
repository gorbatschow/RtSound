#include "RtSoundProvider.h"
#include "RtSoundClient.h"
#include <algorithm>

RtSoundProvider::RtSoundProvider() {}

void RtSoundProvider::addClient(std::weak_ptr<RtSoundClient> client) {
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

void RtSoundProvider::notifyConfigureStream(RtSoundSetup &setup) {
  checkClients();
  for (auto &client : _clients) {
    client.lock().get()->configureStream(setup);
  }
}

void RtSoundProvider::notifyBeforeStartStream(const RtSoundSetup &setup) {
  checkClients();
  for (auto &client : _clients) {
    client.lock().get()->beforeStartStream(setup);
  }
}

void RtSoundProvider::notifyReceiveStreamData(const RtStreamData &data) {
  checkClients();
  for (auto &client : _clients) {
    client.lock().get()->receiveStreamData(data);
  }
}
