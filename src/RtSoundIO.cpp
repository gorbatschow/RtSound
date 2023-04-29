#include "RtSoundIO.h"
#include "RtSoundClient.h"
#include <chrono>
#include <functional>
#include <random>
#include <thread>

namespace RtSound {

IO::IO() {
  initRta(RtAudio::UNSPECIFIED);
}

IO::~IO() = default;

void IO::startSoundEngine(RtAudio::Api api) {
  stopSoundStream();
  initRta(api);
  notifyUpdateSoundDevices();
  setupSoundStream();
}

void IO::setupSoundStream() {
  const auto running{_rta->isStreamRunning()};
  checkClients();
  setupClients();
  notifyUpdateSoundClients();
  orderClients();
  notifyConfigureStream();
  notifyApplyStreamConfig();
  if (running) {
    startSoundStream();
  }
}

void IO::startSoundStream(bool shot) {
  stopSoundStream();

  if (!_streamSetup->inputEnabled() && !_streamSetup->outputEnabled()) {
    return;
  }

  _streamSetup->streamOpts().flags |= RTAUDIO_NONINTERLEAVED;

  const RtAudioErrorType rterr{_rta->openStream(
      _streamSetup->outputStreamPtr(), _streamSetup->inputStreamPtr(),
      RTAUDIO_FLOAT32, _streamSetup->sampleRate(),
      _streamSetup->bufferFramesPtr(), &IO::onHandleStream, this,
      &_streamSetup->streamOpts())};

  if (rterr == RTAUDIO_NO_ERROR) {
    setupSoundStream();
    _streamData->setResult(shot ? 1 : 0);
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

void IO::addClient(std::shared_ptr<Client> client) {
  assert(client.use_count() > 0);
  setupClient(client);
  _clients.push_back(client);
}

void IO::initRta(RtAudio::Api api) {
  _rta = std::make_shared<RtAudio>(api);
  _streamInfo = std::make_shared<StreamInfo>(_rta);
  _streamSetup = std::make_shared<StreamSetup>(_rta);
  _streamData = std::make_shared<StreamData>();
}

void IO::checkClients() {
  std::erase_if(_clients, [](const auto &ptr) { return ptr.use_count() <= 1; });
}

void IO::setupClient(std::shared_ptr<Client> client) {
  client->_streamInfo = _streamInfo;
  client->_streamSetup = _streamSetup;
  client->_streamData = _streamData;
}

void IO::setupClients() {
  const auto func{[this](auto c) { setupClient(c); }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

void IO::orderClients() {
  const auto func{[](const auto &c1, const auto &c2) {
    return c1->clientPriority() < c2->clientPriority();
  }};
  std::sort(std::begin(_clients), std::end(_clients), func);
}

void IO::notifyUpdateSoundClients() const {
  const auto func{[this](const auto c) { c->updateSoundClients(_clients); }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

void IO::notifyUpdateSoundDevices() const {
  const auto func{[this](const auto c) {
    c->updateSoundDevices(_streamSetup->listStreamDevices());
  }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

void IO::notifyConfigureStream() const {
  const auto func{[this](const auto c) { c->configureStream(*_streamSetup); }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

void IO::notifyApplyStreamConfig() const {
  const auto func{[this](const auto c) { c->applyStreamConfig(*_streamSetup); }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

void IO::notifyStreamDataReady() const {
  const auto func{[this](const auto c) { c->streamDataReady(*_streamData); }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

int IO::onHandleStream(void *outputBuffer,
                       void *inputBuffer,
                       unsigned int nFrames,
                       double streamTime,
                       RtAudioStreamStatus streamStatus,
                       void *ioPtr) {
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

  io.notifyStreamDataReady();

  return data.result();
}
} // namespace RtSound
