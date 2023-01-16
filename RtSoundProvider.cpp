#include "RtSoundProvider.h"
#include "RtSoundClient.h"
#include <algorithm>

void RtSoundProvider::addClient(std::weak_ptr<RtSoundClient> client_) {
  const auto client{client_.lock()};
  assert(client);
  client->setStreamProvider(weak_from_this());
  client->setStreamSetup(_streamSetup);
  client->setStreamData(_streamData);
  _clients.push_back(client_);
  if (client->priority() == 0) {
    client->setPriority(_clients.size());
  }
}

void RtSoundProvider::setSetupToData() {
  _streamData->setSoundSetup(streamSetup());
}

void RtSoundProvider::checkClients() {
  std::remove_if(_clients.begin(), _clients.end(),
                 [](const auto &ptr) { return ptr.expired(); });
}

void RtSoundProvider::orderClients() {
  std::sort(_clients.begin(), _clients.end(),
            [](const auto &first, const auto &second) {
              return first.lock().get()->priority() < second.lock()->priority();
            });
}

void RtSoundProvider::notifyUpdateSoundDevices() {
  const auto devices{_streamSetup->listStreamDevices()};
  for (auto &client : _clients) {
    client.lock()->updateSoundDevices(devices);
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
  for (auto &clientPtr : _clients) {
    const auto client{clientPtr.lock()};
    const auto beginTime{std::chrono::high_resolution_clock::now()};
    client->streamDataReady(streamData());
    const auto endTime{std::chrono::high_resolution_clock::now()};
    const auto duration{std::chrono::duration_cast<std::chrono::microseconds>(
                            endTime - beginTime)
                            .count()};
    client->setStreamDataReadyTime(duration);
  }
}
