#pragma once
#include <RtAudio.h>
#include <atomic>
#include <memory>

class RtSoundInfo {
public:
  ~RtSoundInfo() {}

  inline RtAudioStreamStatus streamStatus() const {
    return _streamStatus.load();
  }

  inline double streamTime() const {
    return static_cast<double>(_streamTime.load()) / time_ms;
  }

  inline long processingTime() const { return _processingTime.load(); }

  inline long bufferTime() const { return _bufferTime.load(); }

  inline bool streamRunning() const {
    if (_rta.expired())
      return false;
    return _rta.lock()->isStreamRunning();
  }

  inline bool streamOpen() const {
    if (_rta.expired())
      return false;
    return _rta.lock()->isStreamOpen();
  }

private:
  friend class RtSoundIO;
  RtSoundInfo(std::weak_ptr<RtAudio> rta) : _rta(rta) {}

  inline void setStreamStatus(RtAudioStreamStatus streamStatus) {
    _streamStatus.exchange(streamStatus);
  }
  inline void setStreamTime(double streamTime) {
    _streamTime.exchange(static_cast<unsigned long long>(streamTime * time_ms));
  }
  inline void setProcessingTime(long procTime) {
    _processingTime.exchange(procTime);
  }
  inline void setBufferTime(int nFrames, int sampleRate) {
    _bufferTime.exchange(
        static_cast<long>(double(nFrames) / double(sampleRate) * time_us));
  }

  std::atomic<RtAudioStreamStatus> _streamStatus{};
  std::atomic_ullong _streamTime{};
  std::atomic_long _processingTime{};
  std::atomic_long _bufferTime{};

  const double time_ms{1e3};
  const double time_us{1e6};
  std::weak_ptr<RtAudio> _rta;
};
