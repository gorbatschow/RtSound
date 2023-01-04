#include "RtSoundProvider.h"
#include "RtSoundClient.h"
#include <algorithm>

void RtSoundProvider::addClient(std::weak_ptr<RtSoundClient> client_) {
  const auto client{client_.lock().get()};
  assert(client != nullptr);
  client->setStreamProvider(weak_from_this());
  client->setStreamSetup(_streamSetup);
  client->setStreamData(_streamData);
  _clients.push_back(client_);
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
  _streamDataReadyTime = 0;
  for (auto &client : _clients) {
    const auto ptr{client.lock()};
    const auto beginTime{std::chrono::high_resolution_clock::now()};
    ptr->streamDataReady(streamData());
    const auto endTime{std::chrono::high_resolution_clock::now()};
    const auto duration{std::chrono::duration_cast<std::chrono::microseconds>(
                            endTime - beginTime)
                            .count()};
    ptr->setStreamDataReadyTime(duration);
    _streamDataReadyTime += duration;
  }
}
