#pragma once
#include <RtAudio.h>
#include <mutex>
class RtSoundIO;

struct RtStreamInfo {
  RtAudioStreamStatus streamStatus{0x0};
  double streamTime{0.};
  long processingTime{0};
};

struct RtStreamData {
  RtSoundIO *soundIO{nullptr};
  float *input{nullptr};
  float *output{nullptr};
  unsigned int nFrames;
  int result{0};
  RtStreamInfo info;
  mutable std::mutex mutex;

  inline float *inputBuffer(int channel) const {
    return input + nFrames * channel;
  }
  inline float *outputBuffer(int channel) const {
    return output + nFrames * channel;
  }
};
