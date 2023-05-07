#include "RtSoundIO.h"
#include "RtSoundClient.h"
#include <chrono>
#include <functional>
#include <future>
#include <random>
#include <thread>

namespace RtSound {

struct IO::VirtualStreamArgs {
  std::jthread worker;
  std::vector<float> outputBuffer;
  std::vector<float> inputBuffer;
  RtAudioStreamStatus streamStatus;
  int nFrames{};
  long tFrames{};
  double streamTime{};
  void *ioPtr{};
};

IO::IO() {
  initRta(RtAudio::UNSPECIFIED);
}

IO::~IO() = default;

void IO::startSoundEngine(RtAudio::Api api) {
  initRta(api);
  notifyUpdateSoundDevices();
  setupSoundStream();
}

void IO::setupSoundStream() {
  stopSoundStream();
  checkClients();
  setupClients();
  orderClients();
  notifyUpdateSoundClients();
  notifyConfigureStream();
  notifyApplyStreamConfig();
}

void IO::startSoundStream(bool shot) {
  if (!_streamSetup->inputEnabled() && !_streamSetup->outputEnabled()) {
    return;
  }

  setupSoundStream();
  _streamSetup->streamOpts().flags |= RTAUDIO_NONINTERLEAVED;
  _streamData->setSoundSetup(*_streamSetup);
  _streamData->setResult(shot ? 1 : 0);

  if (!_streamSetup->streamVirtual()) {
    startSoundStreamRta();
  } else {
    startSoundStreamVirtual();
  }
}

void IO::stopSoundStream() {
  stopSoundStreamRta();
  stopSoundStreamVirtual();
}

// -----------------------------------------------------------------------------

void IO::addClient(const std::shared_ptr<Client> &client) {
  assert(client.use_count() > 0);
  setupClient(client);
  _clients.push_back(client);
}

void IO::removeClient(const std::shared_ptr<Client> &client) {
  std::erase_if(_clients, [&client](const auto &ptr) { return ptr == client; });
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

void IO::setupClient(const std::shared_ptr<Client> &client) {
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

// -----------------------------------------------------------------------------

void IO::startSoundStreamRta() {
  _virtualStreamArgs.reset();

  [[maybe_unused]] RtAudioErrorType rterr{
      _rta->openStream(_streamSetup->outputStreamPtr(),
                       _streamSetup->inputStreamPtr(),
                       RTAUDIO_FLOAT32,
                       _streamSetup->sampleRate(),
                       _streamSetup->bufferFramesPtr(),
                       &IO::onHandleStream,
                       this,
                       &_streamSetup->streamOpts())};

  if (rterr == RTAUDIO_NO_ERROR) {
    rterr = _rta->startStream();
  }
}

void IO::startSoundStreamVirtual() {
  _virtualStreamArgs = std::make_unique<VirtualStreamArgs>();
  _virtualStreamArgs->outputBuffer.resize(_streamData->outputBufferSize(), {});
  _virtualStreamArgs->inputBuffer.resize(_streamData->inputBufferSize(), {});
  _virtualStreamArgs->nFrames = _streamData->framesN();
  _virtualStreamArgs->tFrames = _streamData->framesT_us();
  _virtualStreamArgs->ioPtr = this;

  const auto timerEv{[this](std::stop_token stoken) {
    using clock = std::chrono::high_resolution_clock;
    using us_dur = std::chrono::duration<double, std::micro>;

    for (;;) {
      if (stoken.stop_requested()) {
        break;
      }
      const auto tic{clock::now()};
      onHandleStream(
          _virtualStreamArgs->outputBuffer.data(),
          _virtualStreamArgs->inputBuffer.data(), _virtualStreamArgs->nFrames,
          _virtualStreamArgs->streamTime, _virtualStreamArgs->streamStatus,
          _virtualStreamArgs->ioPtr);
      const auto toc{clock::now()};

      const auto t0{us_dur(toc - tic)};
      const auto t1{us_dur(_virtualStreamArgs->tFrames)};
      const auto ts{us_dur(t1 - t0)};
      const double tsc{ts.count()};
      if (tsc > 0) {
        std::this_thread::sleep_for(ts);
        _virtualStreamArgs->streamTime += double(_virtualStreamArgs->tFrames)
                                          * 1e-6;
      } else {
        _virtualStreamArgs->streamTime += double(tsc) * 1e-6;
      }
    }
  }};

  _virtualStreamArgs->worker = std::jthread(timerEv);
  _virtualStreamArgs->worker.detach();
}

void IO::stopSoundStreamRta() {
  if (_rta) {
    if (_rta->isStreamRunning()) {
      _rta->abortStream();
    }
    if (_rta->isStreamOpen()) {
      _rta->closeStream();
    }
  }
}

void IO::stopSoundStreamVirtual() {
  if (_virtualStreamArgs) {
    _virtualStreamArgs->worker.get_stop_source().request_stop();
  }
}

// -----------------------------------------------------------------------------

void IO::notifyUpdateSoundClients() const {
  const auto func{[this](const auto &c) { c->updateSoundClients(_clients); }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

void IO::notifyUpdateSoundDevices() const {
  const auto func{[this](const auto &c) {
    c->updateSoundDevices(_streamSetup->listStreamDevices());
  }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

void IO::notifyConfigureStream() const {
  const auto func{[this](const auto &c) { c->configureStream(*_streamSetup); }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

void IO::notifyApplyStreamConfig() const {
  const auto func{
      [this](const auto &c) { c->applyStreamConfig(*_streamSetup); }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

void IO::notifyStreamDataReady() const {
  const auto func{
      [this](const auto &c) { c->streamDataReadyMeasureTime(*_streamData); }};
  std::for_each(std::cbegin(_clients), std::cend(_clients), func);
}

// -----------------------------------------------------------------------------

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
