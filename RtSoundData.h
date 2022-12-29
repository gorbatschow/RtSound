#pragma once
#include <RtAudio.h>
#include <cstring>
#include <mutex>
class RtSoundIO;

class RtSoundData {
public:
  inline float *inputBuffer(int channel) const {
    if (channel >= _nInputs) {
      throw std::out_of_range("RtStreamData : INPUT channel is out of range");
    }
    return _input + _nFrames * channel;
  }
  inline float *outputBuffer(int channel) const {
    if (channel >= _nOutputs) {
      throw std::out_of_range("RtStreamData : OUTPUT channel is out of range");
    }
    return _output + _nFrames * channel;
  }

  inline void copyInputBuffer(int channel, float *dst) const {
    memcpy(dst, inputBuffer(channel), framesN() * sizeof(float));
  }

  inline void copyOutputBuffer(int channel, float *dst) const {
    memcpy(dst, outputBuffer(channel), framesN() * sizeof(float));
  }

  inline int framesN() const { return _nFrames; }
  inline int inputChannelsN() const { return _nInputs; }
  inline int outputChannelsN() const { return _nOutputs; }
  inline double streamTime() const { return _streamTime; }
  inline int result() const { return _result; }

  mutable std::mutex mutex;

private:
  friend class RtSoundIO;
  friend class RtSoundProvider;
  RtSoundData() {}
  void setInput(float *input) { _input = input; }
  inline void setOutput(float *output) { _output = output; }
  inline void setFramesN(int nFrames) { _nFrames = nFrames; }
  inline void setInputChannelsN(int nInputs) { _nInputs = nInputs; }
  inline void setOutputChannelsN(int nOutputs) { _nOutputs = nOutputs; }
  inline void setStreamTime(double streamTime) { _streamTime = streamTime; }
  inline void setResult(int result) { _result = result; }

private:
  float *_input{nullptr};
  float *_output{nullptr};
  int _nFrames{};
  int _nInputs{};
  int _nOutputs{};
  double _streamTime{};
  int _result{};
};
