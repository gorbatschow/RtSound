#pragma once
#include "RtSoundSetup.h"
#include <RtAudio.h>
#include <cassert>
#include <cstring>
#include <shared_mutex>

class RtSoundData {
public:
  RtSoundData() = default;
  RtSoundData(const RtSoundData &) = delete;
  RtSoundData &operator=(const RtSoundData &) = delete;
  ~RtSoundData() = default;

  inline void setSoundSetup(const RtSoundSetup &setup) {
    setFramesN(setup.bufferFrames());
    setInputsN(setup.inputEnabled() ? setup.inputStream().nChannels : 0);
    setOutputsN(setup.outputEnabled() ? setup.outputStream().nChannels : 0);
  }

  inline void setFramesN(int nFrames) { _nFrames = nFrames; }
  inline int framesN() const { return _nFrames; }

  inline void setStreamTime(double streamTime) { _streamTime = streamTime; }
  inline double streamTime() const { return _streamTime; }

  inline int result() const { return _result; }
  inline void setResult(int result) { _result = result; }

  // Mutex
  mutable std::mutex mutex;

  // Input
  // ---------------------------------------------------------------------------
  inline void setInputBuffer(float *input) { _input = input; }
  inline float *inputBuffer(int channel = 0) const {
    assert(channel < _nInputs);
    return _input + _nFrames * channel;
  }

  inline void nullifyInputBuffer() const {
    memset(inputBuffer(), 0, inputBufferSize() * sizeof(float));
  }

  inline int inputBufferSize() const { return _nInputs * _nFrames; }

  inline void setInputsN(int nInputs) { _nInputs = nInputs; }
  inline int inputsN() const { return _nInputs; }

  inline void copyInput(int channel, float *dst) const {
    memcpy(dst, inputBuffer(channel), framesN() * sizeof(float));
  }

  inline void rcopyInput(int channel, float *dst) const {
    const float *src = inputBuffer(channel);
    int i{0}, n{framesN()};
    for (i = 0; i < n; ++i) {
      dst[n - 1 - i] = src[i];
    }
  }

  // Output
  // ---------------------------------------------------------------------------
  inline void setOutputBuffer(float *output) { _output = output; }
  inline float *outputBuffer(int channel = 0) const {
    assert(channel < _nOutputs);
    return _output + _nFrames * channel;
  }

  inline void nullifyOutputBuffer() const {
    memset(outputBuffer(), 0, outputBufferSize() * sizeof(float));
  }

  inline void setOutputsN(int nOutputs) { _nOutputs = nOutputs; }
  inline int outputsN() const { return _nOutputs; }

  inline int outputBufferSize() const { return _nOutputs * _nFrames; }

  inline void copyOutput(int channel, float *dst) const {
    memcpy(dst, outputBuffer(channel), framesN() * sizeof(float));
  }

  inline void rcopyOutput(int channel, float *dst) const {
    const float *src = outputBuffer(channel);
    int i{0}, n{framesN()};
    for (i = 0; i < n; ++i) {
      dst[n - 1 - i] = src[i];
    }
  }

private:
  float *_input{nullptr};
  float *_output{nullptr};
  int _nFrames{};
  int _nInputs{};
  int _nOutputs{};
  double _streamTime{};
  int _result{};
};
