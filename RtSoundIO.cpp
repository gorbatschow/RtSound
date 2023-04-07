#include "RtSoundIO.h"
#include <chrono>
#include <random>
#include <thread>

namespace RtSound {
void IO::startSoundEngine(RtAudio::Api api) {
  stopSoundStream();

  _rta = std::make_shared<RtAudio>(api);
  _streamInfo.setRtAduio(_rta);
  _streamProvider->setRtAduio(_rta);
  _streamProvider->orderClients();
  _streamProvider->notifyUpdateSoundDevices();
  _streamProvider->notifyConfigureStream();
  _streamProvider->notifyApplyStreamConfig();
}

void IO::setupSoundStream() {
  const auto running{_rta->isStreamRunning()};
  stopSoundStream();
  _streamProvider->orderClients();
  _streamProvider->notifyConfigureStream();
  _streamProvider->notifyApplyStreamConfig();
  if (running) {
    startSoundStream();
  }
}

void IO::startSoundStream(bool shot) {
  if (!_rta) {
    startSoundEngine();
  }

  stopSoundStream();
  _streamProvider->orderClients();
  _streamProvider->notifyConfigureStream();
  _streamProvider->streamSetup().streamOpts().flags |= RTAUDIO_NONINTERLEAVED;
  _streamProvider->streamData().setResult(shot ? 1 : 0);
  _streamProvider->setSetupToData();

  auto &setup{_streamProvider->streamSetup()};
  // assert(setup.inputEnabled() || setup.outputEnabled());
  // assert(setup.inputChannels() > 0 || setup.outputChannels() > 0);
  if (!setup.inputEnabled() && !setup.outputEnabled()) {
    return;
  }
  if (setup.inputChannels() < 1 && setup.outputChannels() < 1) {
    return;
  }

  const RtAudioErrorType rterr{_rta->openStream(setup.outputStreamPtr(),
                                                setup.inputStreamPtr(),
                                                RTAUDIO_FLOAT32,
                                                setup.sampleRate(),
                                                setup.bufferFramesPtr(),
                                                &IO::onHandleStream,
                                                this,
                                                &setup.streamOpts())};

  if (rterr == RTAUDIO_NO_ERROR) {
    _streamProvider->notifyApplyStreamConfig();
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

int IO::onHandleStream(void *outputBuffer,
                       void *inputBuffer,
                       unsigned int nFrames,
                       double streamTime,
                       RtAudioStreamStatus streamStatus,
                       void *ioPtr) {
  auto &io = *static_cast<IO *>(ioPtr);
  auto &data{io._streamProvider->streamData()};
  auto &setup{io.streamProvider().streamSetup()};

  data.setInputBuffer(static_cast<float *>(inputBuffer));
  data.setOutputBuffer(static_cast<float *>(outputBuffer));
  data.setFramesN(int(nFrames));
  data.setStreamTime(streamTime);

  io._streamProvider->notifyStreamDataReady();
  io._streamInfo.setStreamStatus(streamStatus);
  io._streamInfo.setStreamTime(streamTime);

  return data.result();
}
} // namespace RtSound
