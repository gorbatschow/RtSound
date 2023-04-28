#include "RtSoundProvider.h"
#include "RtSoundClient.h"
#include <algorithm>
#include <chrono>

namespace RtSound {

Provider::Provider(const std::vector<std::weak_ptr<Client>> &list)
    : _clients{list} {
  checkClients();
  orderClients();
}

Provider::~Provider() = default;

void Provider::checkClients() {
  std::erase_if(_clients, [](const auto &ptr) { return ptr.expired(); });
}

void Provider::orderClients() {
  std::sort(_clients.begin(), _clients.end(),
            [](const auto &first, const auto &second) {
              return first.lock().get()->priority() < second.lock()->priority();
            });
}

void Provider::notifyUpdateSoundClients(
    const std::vector<std::weak_ptr<Client>> &list) {
  for (auto &client : _clients) {
    client.lock()->updateSoundClients(list);
  }
}

void Provider::notifyUpdateSoundDevices(
    const std::vector<RtAudio::DeviceInfo> &list) {
  for (auto &client : _clients) {
    client.lock()->updateSoundDevices(list);
  }
}

void Provider::notifyConfigureStream(StreamSetup &setup) {
  for (auto &client : _clients) {
    client.lock()->configureStream(setup);
  }
}

void Provider::notifyApplyStreamConfig(const StreamSetup &setup) {
  for (auto &client : _clients) {
    client.lock()->applyStreamConfig(setup);
  }
}

void Provider::notifyStreamDataReady(const StreamData &data) {
  for (auto &clientPtr : _clients) {
    const auto client{clientPtr.lock()};
    const auto beginTime{std::chrono::high_resolution_clock::now()};
    client->streamDataReady(data);
    const auto endTime{std::chrono::high_resolution_clock::now()};
    const auto duration{std::chrono::duration_cast<std::chrono::microseconds>(
                            endTime - beginTime)
                            .count()};
    client->_streamDataReadyTime = duration;
  }
}
} // namespace RtSound
