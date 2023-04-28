#include "RtSoundIO.h"
#include "RtSoundClient.h"
#include "RtSoundProvider.h"
#include <chrono>
#include <random>
#include <thread>

namespace RtSound {
IO::IO() {}

IO::~IO() = default;

void IO::startSoundEngine(RtAudio::Api api) {
  stopSoundStream();

  _rta = std::make_shared<RtAudio>(api);
  _streamInfo = std::make_unique<StreamInfo>(_rta);
  _streamSetup = std::make_unique<StreamSetup>(_rta);
  _streamData = std::make_unique<StreamData>();

  setupSoundStream();
  _streamProvider->notifyUpdateSoundDevices(_streamSetup->listStreamDevices());
}

void IO::setupSoundStream() {
  const auto running{_rta->isStreamRunning()};
  stopSoundStream();
  _streamProvider = std::make_unique<Provider>(_clients);
  _streamProvider->notifyConfigureStream(*_streamSetup);
  _streamProvider->notifyApplyStreamConfig(*_streamSetup);
  _clients = _streamProvider->clients();
  if (running) {
    startSoundStream();
  }
}

void IO::startSoundStream(bool shot) {
  // TODO assert or exception...
  if (!_rta) {
    return;
  }

  stopSoundStream();

  if (!_streamSetup->inputEnabled() && !_streamSetup->outputEnabled()) {
    return;
  }
  if (_streamSetup->inputChannels() < 1 && _streamSetup->outputChannels() < 1) {
    return;
  }

  _streamSetup->streamOpts().flags |= RTAUDIO_NONINTERLEAVED;

  const RtAudioErrorType rterr{_rta->openStream(
      _streamSetup->outputStreamPtr(), _streamSetup->inputStreamPtr(),
      RTAUDIO_FLOAT32, _streamSetup->sampleRate(),
      _streamSetup->bufferFramesPtr(), &IO::onHandleStream, this,
      &_streamSetup->streamOpts())};

  if (rterr == RTAUDIO_NO_ERROR) {
    _streamProvider = std::make_unique<Provider>(_clients);
    _streamProvider->notifyConfigureStream(*_streamSetup);
    _streamProvider->notifyApplyStreamConfig(*_streamSetup);
    _streamData->setResult(shot ? 1 : 0);
    _clients = _streamProvider->clients();
    _rta->startStream();
  }
}

void IO::stopSoundStream() {
  if (!_rta) {
    return;
  }

  if (_rta->isStreamRunning()) {
    _rta->stopStream();
  }

  if (_rta->isStreamOpen()) {
    _rta->closeStream();
  }
}

void IO::addClient(std::weak_ptr<Client> client) {
  client.lock()->_streamInfo = _streamInfo;
  _clients.push_back(client);
}

int IO::onHandleStream(void *outputBuffer, void *inputBuffer,
                       unsigned int nFrames, double streamTime,
                       RtAudioStreamStatus streamStatus, void *ioPtr) {
  auto &io = *static_cast<IO *>(ioPtr);
  auto &data{*io._streamData};
  auto &setup{*io._streamSetup};
  auto &info{*io._streamInfo};

  data.setInputBuffer(static_cast<float *>(inputBuffer));
  data.setOutputBuffer(static_cast<float *>(outputBuffer));
  data.setFramesN(int(nFrames));
  data.setStreamTime(streamTime);

  info.setStreamStatus(streamStatus);
  info.setStreamTime(streamTime);

  io._streamProvider->notifyStreamDataReady(data);

  return data.result();
}
} // namespace RtSound
