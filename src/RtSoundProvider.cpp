#include "RtSoundProvider.h"
#include "RtSoundClient.h"
#include <algorithm>

namespace RtSound {
void Provider::addClient(std::weak_ptr<Client> client_) {
  const auto client{client_.lock()};
  assert(client);
  client->setStreamProvider(weak_from_this());
  client->setStreamSetup(_streamSetup);
  client->setStreamData(_streamData);
  client->setStreamInfo(_streamInfo);
  _clients.push_back(client_);
  if (client->priority() == 0) {
    client->setPriority(_clients.size());
  }
}

void Provider::setSetupToData() {
  _streamData->setSoundSetup(streamSetup());
}

void Provider::checkClients() {
  std::erase_if(_clients, [](const auto &ptr) { return ptr.expired(); });
}

void Provider::orderClients() {
  std::sort(_clients.begin(), _clients.end(),
            [](const auto &first, const auto &second) {
              return first.lock().get()->priority() < second.lock()->priority();
            });
}

void Provider::notifyUpdateSoundDevices() {
  const auto devices{_streamSetup->listStreamDevices()};
  for (auto &client : _clients) {
    client.lock()->updateSoundDevices(devices);
  }
}

void Provider::notifyConfigureStream() {
  for (auto &client : _clients) {
    client.lock()->configureStream(streamSetup());
  }
}

void Provider::notifyApplyStreamConfig() {
  for (auto &client : _clients) {
    client.lock()->applyStreamConfig(streamSetup());
  }
}

void Provider::notifyStreamDataReady() {
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
} // namespace RtSound
