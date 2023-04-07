#pragma once
#include "RtSoundData.h"
#include "RtSoundSetup.h"
#include <cassert>
#include <memory>

namespace RtSound {
class Provider;

class Client
{
  friend class Provider;

public:
  Client(int priority = 0)
      : _priority(priority) {}
  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;
  virtual ~Client() = default;

  inline void setPriority(int priority) { _priority = priority; }
  inline int priority() const { return _priority; };

  inline void setClientName(const std::string &name) { _clientName = name; }
  inline const std::string &clientName() const { return _clientName; }

  virtual const std::type_info &clientTypeId() const { return typeid(this); }

  inline const std::uintptr_t clientId() const {
    return reinterpret_cast<std::uintptr_t>(this);
  }

  const Provider &streamProvider() const {
    const auto ptr{_streamProvider.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }
  inline bool hasStreamProvider() const {
    return _streamProvider.lock() != nullptr;
  }

  const Setup &streamSetup() const {
    const auto ptr{_streamSetup.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }
  inline bool hasStreamSetup() const { return _streamSetup.lock() != nullptr; }

  const Data &streamData() const {
    const auto ptr{_streamData.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }
  inline bool hasStreamData() const { return _streamData.lock() != nullptr; }
  inline long streamDataReadyTime() const { return _streamDataReadyTime; }

protected:
  virtual void updateSoundDevices(const std::vector<RtAudio::DeviceInfo> &) {}
  virtual void configureStream(Setup &) {}
  virtual void applyStreamConfig(const Setup &) {}
  virtual void streamDataReady(const Data &) {}

  std::mutex clientMutex;

private:
  inline void setStreamProvider(std::weak_ptr<Provider> provider) {
    assert(provider.lock() != nullptr);
    _streamProvider.swap(provider);
  }

  inline void setStreamSetup(std::weak_ptr<Setup> setup) {
    assert(setup.lock() != nullptr);
    _streamSetup.swap(setup);
  }

  inline void setStreamData(std::weak_ptr<Data> data) {
    assert(data.lock() != nullptr);
    _streamData.swap(data);
  }

  inline void setStreamDataReadyTime(long t) { _streamDataReadyTime = t; }

  int _priority{};
  std::string _clientName{"Sound Client"};
  std::weak_ptr<Provider> _streamProvider{};
  std::weak_ptr<Setup> _streamSetup{};
  std::weak_ptr<Data> _streamData{};
  long _streamDataReadyTime{};
};
} // namespace RtSound
