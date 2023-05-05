#pragma once
#include <RtAudio.h>
#include <memory>

namespace RtSound {
class StreamInfo {
public:
  StreamInfo(std::shared_ptr<RtAudio> rta) : _rta{rta} {}
  StreamInfo(const StreamInfo &) = delete;
  StreamInfo &operator=(const StreamInfo &) = delete;
  ~StreamInfo() = default;

  // Stream status
  inline void setStreamStatus(RtAudioStreamStatus streamStatus) {
    _streamStatus = streamStatus;
  }
  inline RtAudioStreamStatus streamStatus() const { return _streamStatus; }

  // Stream time
  inline void setStreamTime(double streamTime) {
    _streamTime = static_cast<unsigned long long>(streamTime * time_ms);
  }
  inline double streamTime() const {
    return static_cast<double>(_streamTime / time_ms);
  }

  // Stream running
  inline bool streamRunning() const { return _rta->isStreamRunning(); }

  // Stream open
  inline bool streamOpen() const { return _rta->isStreamOpen(); }

private:
  std::shared_ptr<RtAudio> _rta;
  RtAudioStreamStatus _streamStatus{};
  unsigned long long _streamTime{};
  const double time_ms{1e3};
};
} // namespace RtSound
