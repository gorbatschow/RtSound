#pragma once
#include <RtAudio.h>
#include <atomic>
#include <cassert>
#include <memory>

class RtSoundInfo {
public:
  RtSoundInfo() = default;
  RtSoundInfo(const RtSoundInfo &) = delete;
  RtSoundInfo &operator=(const RtSoundInfo &) = delete;
  ~RtSoundInfo() = default;

  inline void setRtAduio(std::weak_ptr<RtAudio> rta) {
    assert(rta.lock() != nullptr);
    _rta.swap(rta);
  }

  // Stream status
  inline void setStreamStatus(RtAudioStreamStatus streamStatus) {
    _streamStatus.exchange(streamStatus);
  }
  inline RtAudioStreamStatus streamStatus() const {
    return _streamStatus.load();
  }

  // Stream time
  inline void setStreamTime(double streamTime) {
    _streamTime.exchange(static_cast<unsigned long long>(streamTime * time_ms));
  }
  inline double streamTime() const {
    return static_cast<double>(_streamTime.load()) / time_ms;
  }

  // Stream running
  inline bool streamRunning() const {
    const auto rta{_rta.lock().get()};
    if (!rta)
      return false;
    return rta->isStreamRunning();
  }

  // Stream open
  inline bool streamOpen() const {
    const auto rta{_rta.lock().get()};
    if (!rta)
      return false;
    return rta->isStreamOpen();
  }

private:
  std::weak_ptr<RtAudio> _rta;
  std::atomic<RtAudioStreamStatus> _streamStatus{};
  std::atomic_long _streamTime{};
  const double time_ms{1e3};
};
