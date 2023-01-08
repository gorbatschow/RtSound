#pragma once
#include "RtSoundData.h"
#include "RtSoundSetup.h"
#include <cassert>
#include <memory>
class RtSoundProvider;

class RtSoundClient {
  friend class RtSoundProvider;

public:
  RtSoundClient(int priority = 0) : _priority(priority) {}
  RtSoundClient(const RtSoundClient &) = delete;
  RtSoundClient &operator=(const RtSoundClient &) = delete;
  virtual ~RtSoundClient() = default;

  inline void setPriority(int priority) { _priority = priority; }
  inline int priority() const { return _priority; };

  inline void setClientName(const std::string &name) { _clientName = name; }
  inline const std::string &clientName() const { return _clientName; }

  virtual const std::type_info &clientTypeId() const { return typeid(this); }

  inline const std::uintptr_t clientId() const {
    return reinterpret_cast<std::uintptr_t>(this);
  }

  const RtSoundProvider &streamProvider() const {
    const auto ptr{_streamProvider.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }
  inline bool hasStreamProvider() const {
    return _streamProvider.lock() != nullptr;
  }

  const RtSoundSetup &streamSetup() const {
    const auto ptr{_streamSetup.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }
  inline bool hasStreamSetup() const { return _streamSetup.lock() != nullptr; }

  const RtSoundData &streamData() const {
    const auto ptr{_streamData.lock()};
    assert(ptr != nullptr);
    return (*ptr);
  }
  inline bool hasStreamData() const { return _streamData.lock() != nullptr; }
  inline long streamDataReadyTime() const { return _streamDataReadyTime; }

protected:
  virtual void updateSoundDevices(const std::vector<RtAudio::DeviceInfo> &) {}
  virtual void configureStream(RtSoundSetup &) {}
  virtual void applyStreamConfig(const RtSoundSetup &) {}
  virtual void streamDataReady(const RtSoundData &) {}

private:
  inline void setStreamProvider(std::weak_ptr<RtSoundProvider> provider) {
    assert(provider.lock() != nullptr);
    _streamProvider.swap(provider);
  }

  inline void setStreamSetup(std::weak_ptr<RtSoundSetup> setup) {
    assert(setup.lock() != nullptr);
    _streamSetup.swap(setup);
  }

  inline void setStreamData(std::weak_ptr<RtSoundData> data) {
    assert(data.lock() != nullptr);
    _streamData.swap(data);
  }

  inline void setStreamDataReadyTime(long t) { _streamDataReadyTime = t; }

  int _priority{};
  std::string _clientName{"Sound Client"};
  std::weak_ptr<RtSoundProvider> _streamProvider{};
  std::weak_ptr<RtSoundSetup> _streamSetup{};
  std::weak_ptr<RtSoundData> _streamData{};
  long _streamDataReadyTime{};
};
