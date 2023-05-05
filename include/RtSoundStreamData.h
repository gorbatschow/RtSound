#pragma once
#include "RtSoundStreamSetup.h"
#include <algorithm>
#include <cassert>
#include <cstring>

namespace RtSound {
class StreamData {
public:
  StreamData() = default;
  StreamData(const StreamData &) = delete;
  StreamData &operator=(const StreamData &) = delete;
  ~StreamData() = default;

  // Sound Setup
  inline void setSoundSetup(const StreamSetup &setup) {
    setFramesN(setup.bufferFrames());
    setInputsN(setup.inputEnabled() ? setup.inputStream().nChannels : 0);
    setOutputsN(setup.outputEnabled() ? setup.outputStream().nChannels : 0);
    setSampleRate(setup.sampleRate());
  }

  // Frame Count
  inline void setFramesN(int nFrames) { _nFrames = nFrames; }
  inline int framesN() const { return _nFrames; }
  inline double framesT() const {
    return double(_nFrames) / double(_sampleRate);
  }
  inline long framesT_us() const {
    return long(double(_nFrames) / double(_sampleRate) * 1e6);
  }
  inline size_t framesBytes() const { return sizeof(float) * _nFrames; }

  // Sample Rate
  inline void setSampleRate(int sampleRate) { _sampleRate = sampleRate; }
  inline int sampleRate() const { return _sampleRate; }

  // Stream Time
  inline void setStreamTime(double streamTime) { _streamTime = streamTime; }
  inline double streamTime() const { return _streamTime; }

  // Result
  inline int result() const { return _result; }
  inline void setResult(int result) { _result = result; }

  // Check Channel
  bool hasChannel(bool isInput, int channel) const {
    return isInput ? _nInputs > channel : _nOutputs > channel;
  }

  // Input
  // ---------------------------------------------------------------------------
  inline void setInputBuffer(float *input) { _input = input; }
  inline float *inputBuffer(int channel = 0, int bias = 0) const {
    assert(channel < _nInputs);
    return _input + _nFrames * channel + bias;
  }

  inline void nullifyInputBuffer() const {
    memset(inputBuffer(), 0, inputBufferSize() * sizeof(float));
    // std::fill(inputBuffer(), inputBuffer() + inputBufferSize(), 0.f);
  }

  inline int inputBufferSize() const { return _nInputs * _nFrames; }

  inline void setInputsN(int nInputs) { _nInputs = nInputs; }
  inline int inputsN() const { return _nInputs; }

  // Output
  // ---------------------------------------------------------------------------
  inline void setOutputBuffer(float *output) {
    _output = output;
    if (_output != nullptr) {
      nullifyOutputBuffer();
    }
  }
  inline float *outputBuffer(int channel = 0, int bias = 0) const {
    assert(channel < _nOutputs);
    return _output + _nFrames * channel + bias;
  }

  inline void nullifyOutputBuffer() const {
    memset(outputBuffer(), 0, outputBufferSize() * sizeof(float));
  }

  inline void setOutputsN(int nOutputs) { _nOutputs = nOutputs; }
  inline int outputsN() const { return _nOutputs; }

  inline int outputBufferSize() const { return _nOutputs * _nFrames; }

private:
  float *_input{nullptr};
  float *_output{nullptr};
  int _nFrames{};
  int _nInputs{};
  int _nOutputs{};
  int _sampleRate{};
  double _streamTime{};
  int _result{};
};
} // namespace RtSound
