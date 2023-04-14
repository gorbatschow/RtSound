#pragma once
#include "RtSoundProvider.h"
#include "RtSoundStreamData.h"
#include "RtSoundStreamInfo.h"
#include "RtSoundStreamSetup.h"
#include <cassert>
#include <memory>

namespace RtSound {
class Provider;
class Client : public std::enable_shared_from_this<Client> {
  friend class Provider;

public:
  Client(int priority = 0) : _priority(priority) {}
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

  // Provider
  const Provider &streamProvider() const {
    const auto ptr{_streamProvider.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }
  inline bool hasStreamProvider() const {
    return _streamProvider.lock() != nullptr;
  }

  // Stream Setup
  const StreamSetup &streamSetup() const {
    const auto ptr{_streamSetup.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }
  inline bool hasStreamSetup() const { return _streamSetup.lock() != nullptr; }

  // Stream Data
  const StreamData &streamData() const {
    const auto ptr{_streamData.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }
  inline bool hasStreamData() const { return _streamData.lock() != nullptr; }
  inline long streamDataReadyTime() const { return _streamDataReadyTime; }

  // Stream Info
  const StreamInfo &streamInfo() const {
    const auto ptr{_streamInfo.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }

protected:
  virtual void applyStreamProvider(Provider &) {}
  virtual void updateSoundDevices(const std::vector<RtAudio::DeviceInfo> &) {}
  virtual void configureStream(StreamSetup &) {}
  virtual void applyStreamConfig(const StreamSetup &) {}
  virtual void streamDataReady(const StreamData &) {}

  std::mutex clientMutex;

private:
  inline void setStreamProvider(std::weak_ptr<Provider> provider) {
    assert(!provider.expired());
    _streamProvider.swap(provider);
  }

  inline void setStreamSetup(std::weak_ptr<StreamSetup> setup) {
    assert(!setup.expired());
    _streamSetup.swap(setup);
  }

  inline void setStreamData(std::weak_ptr<StreamData> data) {
    assert(!data.expired());
    _streamData.swap(data);
  }

  inline void setStreamInfo(std::weak_ptr<StreamInfo> info) {
    assert(!info.expired());
    _streamInfo.swap(info);
  }

  inline void setStreamDataReadyTime(long t) { _streamDataReadyTime = t; }

  int _priority{};
  std::string _clientName{"Sound Client"};
  std::weak_ptr<Provider> _streamProvider;
  std::weak_ptr<StreamSetup> _streamSetup;
  std::weak_ptr<StreamData> _streamData;
  std::weak_ptr<StreamInfo> _streamInfo;
  long _streamDataReadyTime{};
};
} // namespace RtSound
