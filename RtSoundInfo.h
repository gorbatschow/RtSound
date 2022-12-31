#pragma once
#include <RtAudio.h>
#include <atomic>
#include <cassert>
#include <memory>

class RtSoundInfo {
public:
  RtSoundInfo() = default;
  ~RtSoundInfo() = default;

  inline void setRtAduio(std::weak_ptr<RtAudio> rta) {
    assert(rta.lock() != nullptr);
    _rta.swap(rta);
  }

  inline void setStreamStatus(RtAudioStreamStatus streamStatus) {
    _streamStatus.exchange(streamStatus);
  }
  inline RtAudioStreamStatus streamStatus() const {
    return _streamStatus.load();
  }

  inline void setStreamTime(double streamTime) {
    _streamTime.exchange(static_cast<unsigned long long>(streamTime * time_ms));
  }
  inline double streamTime() const {
    return static_cast<double>(_streamTime.load()) / time_ms;
  }

  inline void setProcessingTime(long procTime) {
    _processingTime.exchange(procTime);
  }
  inline long processingTime() const { return _processingTime.load(); }

  inline void setBufferTime(int nFrames, int sampleRate) {
    _bufferTime.exchange(
        static_cast<long>(double(nFrames) / double(sampleRate) * time_us));
  }
  inline long bufferTime() const { return _bufferTime.load(); }

  inline bool streamRunning() const {
    const auto rta{_rta.lock().get()};
    if (!rta)
      return false;
    return rta->isStreamRunning();
  }

  inline bool streamOpen() const {
    const auto rta{_rta.lock().get()};
    if (!rta)
      return false;
    return rta->isStreamOpen();
  }

private:
  std::weak_ptr<RtAudio> _rta;
  std::atomic<RtAudioStreamStatus> _streamStatus{};
  std::atomic_ullong _streamTime{};
  std::atomic_long _processingTime{};
  std::atomic_long _bufferTime{};
  const double time_ms{1e3};
  const double time_us{1e6};
};
