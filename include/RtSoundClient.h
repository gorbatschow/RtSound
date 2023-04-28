#pragma once
#include "RtSoundStreamData.h"
#include "RtSoundStreamInfo.h"
#include "RtSoundStreamSetup.h"
#include <cassert>
#include <memory>

namespace RtSound {
class Provider;
class StreamInfo;

class Client {
  friend class IO;
  friend class Provider;

public:
  Client() = default;
  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;
  virtual ~Client() = default;

  virtual const std::type_info &clientTypeId() const { return typeid(this); }

  inline const std::uintptr_t clientId() const {
    return reinterpret_cast<std::uintptr_t>(this);
  }

  inline void setClientName(const std::string &name) { _clientName = name; }
  inline const std::string &clientName() const { return _clientName; }

  inline void setPriority(int priority) { _priority = priority; }
  inline int priority() const { return _priority; };

  inline const StreamInfo &streamInfo() const { return *_streamInfo; }
  inline long streamDataReadyTime() const { return _streamDataReadyTime; }

protected:
  virtual void updateSoundDevices(const std::vector<RtAudio::DeviceInfo> &) {}
  virtual void configureStream(StreamSetup &) {}
  virtual void applyStreamConfig(const StreamSetup &) {}
  virtual void updateSoundClients(const std::vector<std::weak_ptr<Client>> &){};
  virtual void streamDataReady(const StreamData &) {}

  mutable std::mutex clientMutex;

private:
  std::string _clientName{"Sound Client"};
  int _priority{};
  long _streamDataReadyTime{};
  std::shared_ptr<StreamInfo> _streamInfo;
};
} // namespace RtSound
