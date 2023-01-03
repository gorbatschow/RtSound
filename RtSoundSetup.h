#pragma once
#include <RtAudio.h>
#include <cassert>
#include <memory>

class RtSoundSetup {
public:
  RtSoundSetup() = default;
  ~RtSoundSetup() = default;

  void setRtAduio(std::weak_ptr<RtAudio> rta) {
    assert(rta.lock() != nullptr);
    _rta.swap(rta);
    _inputStream.deviceId = _rta.lock()->getDefaultInputDevice();
    _outputStream.deviceId = _rta.lock()->getDefaultOutputDevice();
  }

  // List Devices
  std::vector<RtAudio::DeviceInfo> listStreamDevices() const {
    const auto rta{_rta.lock().get()};
    if (!rta)
      return {};
    std::vector<RtAudio::DeviceInfo> devices;
    const auto ids{rta->getDeviceIds()};
    devices.reserve(ids.size());
    for (auto &id : ids) {
      devices.push_back(rta->getDeviceInfo(id));
    }
    return devices;
  }

  // Sound API
  inline RtAudio::Api soundApi() {
    const auto rta{_rta.lock().get()};
    if (!rta)
      return RtAudio::Api::UNSPECIFIED;
    return rta->getCurrentApi();
  }
  inline std::string soundApiName() {
    return RtAudio::getApiDisplayName(soundApi());
  }

  // Sample Rate
  inline void setSampleRate(int sampleRate) { _sampleRate = sampleRate; }
  inline int sampleRate() const { return _sampleRate; }

  // Buffer Frames
  inline void setBufferFrames(int bufferFrames) {
    _bufferFrames = bufferFrames;
  }
  inline unsigned int bufferFrames() const { return _bufferFrames; }
  inline unsigned int *bufferFramesPtr() { return &_bufferFrames; }

  // Helpers
  inline double frequencyResolution() const {
    return double(_sampleRate) / double(_bufferFrames);
  }
  inline double timeResolution() const { return 1.0 / double(_sampleRate); }

  // Stream Options
  // ---------------------------------------------------------------------------

  // Buffers Num
  inline void setBuffersNum(unsigned int nBuffers) {
    _streamOpts.numberOfBuffers = nBuffers;
  }
  unsigned int buffersNum() const { return _streamOpts.numberOfBuffers; }

  // Flags
  inline void clearStreamFlags() { _streamOpts.flags = 0; }
  inline void addStreamFlags(RtAudioStreamFlags flags) {
    _streamOpts.flags |= flags;
  }
  inline RtAudioStreamFlags streamFlags() const { return _streamOpts.flags; }
  inline bool checkStreamFlags(RtAudioStreamFlags flags) {
    return _streamOpts.flags & flags;
  }

  // Options
  inline RtAudio::StreamOptions &streamOpts() { return _streamOpts; }

  // Input Stream
  // ---------------------------------------------------------------------------

  // Input Enabled
  inline void setInputEnabled(bool enabled) { _inputEnabled = enabled; }
  inline bool inputEnabled() const { return _inputEnabled; }

  // Input Device Id
  inline void setInputDeviceId(unsigned int id) { _inputStream.deviceId = id; }
  inline unsigned int inputDeviceId() const { return _inputStream.deviceId; }

  // Input Device Name
  inline std::string inputDeviceName() const {
    const auto rta{_rta.lock().get()};
    if (!rta || !_inputEnabled)
      return {};
    return rta->getDeviceInfo(_inputStream.deviceId).name;
  }

  // Input Channels Num
  inline void setInputChannels(unsigned int channels) {
    _inputStream.nChannels = channels;
  }
  inline unsigned int inputChannels() const { return _inputStream.nChannels; }

  // First Input Channel
  inline void setInputFirstChannel(unsigned int channel) {
    _inputStream.firstChannel = channel;
  }
  inline unsigned int inputFirstChannel() const {
    return _inputStream.firstChannel;
  }

  // Input Stream Parameters
  inline const RtAudio::StreamParameters &inputStream() const {
    return _inputStream;
  }
  inline RtAudio::StreamParameters &inputStream() { return _inputStream; }
  inline RtAudio::StreamParameters *inputStreamPtr() {
    return _inputEnabled ? &_inputStream : nullptr;
  }

  // Output Stream
  // ---------------------------------------------------------------------------

  // Output Enabled
  inline void setOutputEnabled(bool enabled) { _outputEnabled = enabled; }
  inline bool outputEnabled() const { return _outputEnabled; }

  // Output Device Id
  inline void setOutputDeviceId(unsigned int id) {
    _outputStream.deviceId = id;
  }
  inline unsigned int outputDeviceId() const { return _outputStream.deviceId; }

  // Output Device Name
  inline std::string outputDeviceName() const {
    const auto rta{_rta.lock().get()};
    if (!rta || !_outputEnabled)
      return {};
    return rta->getDeviceInfo(_outputStream.deviceId).name;
  }

  // Output Channels Num
  inline void setOutputChannels(unsigned int channels) {
    _outputStream.nChannels = channels;
  }
  inline unsigned int outputChannels() const { return _outputStream.nChannels; }

  // First Output Channel
  inline void setOutputFirstChannel(unsigned int channel) {
    _outputStream.firstChannel = channel;
  }
  inline unsigned int outputFirstChannel() const {
    return _outputStream.firstChannel;
  }

  // Output Stream Parameters
  inline const RtAudio::StreamParameters &outputStream() const {
    return _outputStream;
  }
  inline RtAudio::StreamParameters &outputStream() { return _outputStream; }
  inline RtAudio::StreamParameters *outputStreamPtr() {
    return _outputEnabled ? &_outputStream : nullptr;
  }

private:
  std::weak_ptr<RtAudio> _rta;
  int _sampleRate{48000};
  unsigned int _bufferFrames{1024};
  bool _inputEnabled{};
  bool _outputEnabled{};
  RtAudio::StreamOptions _streamOpts;
  RtAudio::StreamParameters _inputStream;
  RtAudio::StreamParameters _outputStream;
};
