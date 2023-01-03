#include "RtSoundIO.h"
#include <chrono>
#include <random>
#include <thread>

void RtSoundIO::startSoundEngine(RtAudio::Api api) {
  stopSoundStream();

  _rta = std::make_shared<RtAudio>(api);
  _streamInfo.setRtAduio(_rta);
  _streamProvider.setRtAduio(_rta);
  _streamProvider.notifyUpdateSoundDevices();
  _streamProvider.notifyApplyStreamConfig();
}

void RtSoundIO::startSoundStream(bool shot) {
  if (!_rta) {
    startSoundEngine();
  }

  stopSoundStream();
  _streamProvider.orderClients();
  _streamProvider.notifyConfigureStream();
  _streamProvider.streamSetup().streamOpts().flags |= RTAUDIO_NONINTERLEAVED;
  _streamProvider.streamData().setSoundSetup(_streamProvider.streamSetup());
  _streamProvider.streamData().setResult(shot ? 1 : 0);

  auto &setup{_streamProvider.streamSetup()};
  assert(!setup.inputEnabled() || !setup.outputEnabled());
  assert(setup.inputChannels() > 0 || setup.outputChannels() > 0);

  const RtAudioErrorType rterr{_rta->openStream(
      setup.outputStreamPtr(), setup.inputStreamPtr(), RTAUDIO_FLOAT32,
      setup.sampleRate(), setup.bufferFramesPtr(), &RtSoundIO::onHandleStream,
      this, &setup.streamOpts())};

  if (rterr == RTAUDIO_NO_ERROR) {
    _streamProvider.notifyApplyStreamConfig();
    _rta->startStream();
  }
}

void RtSoundIO::stopSoundStream() {
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

int RtSoundIO::onHandleStream(void *outputBuffer, void *inputBuffer,
                              unsigned int nFrames, double streamTime,
                              RtAudioStreamStatus streamStatus, void *ioPtr) {

  auto &io = *static_cast<RtSoundIO *>(ioPtr);
  auto &data{io._streamProvider.streamData()};
  auto &setup{io.streamProvider().streamSetup()};

  data.setInputBuffer(static_cast<float *>(inputBuffer));
  data.setOutputBuffer(static_cast<float *>(outputBuffer));
  data.setFramesN(int(nFrames));
  data.setStreamTime(streamTime);

  const auto beginTime{std::chrono::high_resolution_clock::now()};
  io._streamProvider.notifyStreamDataReady();
  const auto endTime{std::chrono::high_resolution_clock::now()};
  const auto duration{std::chrono::duration_cast<std::chrono::microseconds>(
      endTime - beginTime)};

  io._streamInfo.setStreamStatus(streamStatus);
  io._streamInfo.setStreamTime(streamTime);
  io._streamInfo.setProcessingTime(duration.count());
  io._streamInfo.setBufferTime(nFrames, setup.sampleRate());

  return data.result();
}
