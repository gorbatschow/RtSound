#pragma once
#include <RtAudio.h>

class RtSoundSetup {
public:
  RtSoundSetup() {
    _inputStream.nChannels = 2;
    _outputStream.nChannels = 2;
    _streamOpts.flags |= RTAUDIO_NONINTERLEAVED;
    _streamOpts.flags |= RTAUDIO_SCHEDULE_REALTIME;
    _streamOpts.flags |= RTAUDIO_MINIMIZE_LATENCY;
    _streamOpts.flags |= RTAUDIO_HOG_DEVICE;
    _streamOpts.priority = 0;
    _streamOpts.numberOfBuffers = 1;
    _streamOpts.streamName = "RtSoundIO";
  }

  inline void setSampleRate(int sampleRate) { _sampleRate = sampleRate; }
  inline int sampleRate() const { return _sampleRate; }

  inline void setBufferFrames(int bufferFrames) {
    _bufferFrames = bufferFrames;
  }
  inline unsigned int bufferFrames() const { return _bufferFrames; }
  inline unsigned int *bufferFramesPtr() { return &_bufferFrames; }

  inline void setInputEnabled(bool enabled) { _inputEnabled = enabled; }
  inline bool inputEnabled() const { return _inputEnabled; }

  inline void setOutputEnabled(bool enabled) { _outputEnabled = enabled; }
  inline bool outputEnabled() const { return _outputEnabled; }

  inline RtAudio::StreamOptions &streamOpts() { return _streamOpts; }

  inline const RtAudio::StreamParameters &inputStream() const {
    return _inputStream;
  }
  inline RtAudio::StreamParameters &inputStream() { return _inputStream; }
  inline RtAudio::StreamParameters *inputStreamPtr() {
    return _inputEnabled ? &_inputStream : nullptr;
  }

  inline const RtAudio::StreamParameters &outputStream() const {
    return _outputStream;
  }
  inline RtAudio::StreamParameters &outputStream() { return _outputStream; }
  inline RtAudio::StreamParameters *outputStreamPtr() {
    return _outputEnabled ? &_outputStream : nullptr;
  }

  inline double frequencyResolution() const {
    return double(_sampleRate) / double(_bufferFrames);
  }
  inline double timeResolution() const { return 1.0 / double(_sampleRate); }

private:
  int _sampleRate{48000};
  unsigned int _bufferFrames{1024};
  bool _inputEnabled{true};
  bool _outputEnabled{false};
  RtAudio::StreamOptions _streamOpts;
  RtAudio::StreamParameters _inputStream;
  RtAudio::StreamParameters _outputStream;
};
