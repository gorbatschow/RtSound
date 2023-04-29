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

  inline void setClientPriority(int priority) {
    _priority = priority;
    priorityChanged(_priority);
  }
  inline int clientPriority() const { return _priority; };

  inline const StreamInfo &streamInfo() const { return *_streamInfo; }
  inline const StreamSetup &streamSetup() const { return *_streamSetup; }
  inline const StreamData &streamData() const { return *_streamData; }

  inline long streamDataReadyTime() const { return _streamDataReadyTime; }

protected:
  virtual void priorityChanged(int p) {}
  virtual void updateSoundClients(
      const std::vector<std::shared_ptr<Client> > &){};
  virtual void updateSoundDevices(const std::vector<RtAudio::DeviceInfo> &) {}
  virtual void configureStream(StreamSetup &) {}
  virtual void applyStreamConfig(const StreamSetup &) {}
  virtual void streamDataReady(const StreamData &) {}

  mutable std::mutex clientMutex;

private:
  std::string _clientName{"Sound Client"};
  int _priority{};
  long _streamDataReadyTime{};
  std::shared_ptr<StreamInfo> _streamInfo;
  std::shared_ptr<StreamSetup> _streamSetup;
  std::shared_ptr<StreamData> _streamData;
};
} // namespace RtSound
