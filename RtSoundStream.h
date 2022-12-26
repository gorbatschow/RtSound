#pragma once
#include <RtAudio.h>
#include <mutex>
class RtSoundIO;

struct RtStreamInfo {
  RtAudioStreamStatus streamStatus{};
  double streamTime{};
  long processingTime{};
};

struct RtStreamData {
  RtSoundIO *soundIO{nullptr};
  float *input{nullptr};
  float *output{nullptr};
  int nFrames{};
  int nInputChannels{};
  int nOutputChannels{};
  double streamTime{};
  int result{};
  mutable std::mutex mutex;

  inline float *inputBuffer(int channel) const {
    return input + nFrames * channel;
  }
  inline float *outputBuffer(int channel) const {
    return output + nFrames * channel;
  }
};
