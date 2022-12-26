#include "RtSoundIO.h"
#include <RtSoundClient.h>
#include <chrono>
#include <random>
#include <thread>

RtSoundIO::RtSoundIO() {}

void RtSoundIO::startAudioEngine(RtAudio::Api api) {
  if (_rta) {
    _rta->stopStream();
  }
  _rta = std::make_unique<RtAudio>(api);
  _nextSetup.inputStream().deviceId = _rta->getDefaultInputDevice();
  _nextSetup.outputStream().deviceId = _rta->getDefaultOutputDevice();
  _currSetup = _nextSetup;
}

void RtSoundIO::startAudioStream(bool shot) {
  stopAudioStream();
  orderClients();
  notifyConfigureStream(_nextSetup);

  _currSetup = _nextSetup;
  _currSetupPub = _currSetup;
  _streamData.nInputChannels = _currSetup.inputStream().nChannels;
  _streamData.nOutputChannels = _currSetup.outputStream().nChannels;
  _streamData.result = shot ? 1 : 0;

  const RtAudioErrorType rterr{_rta->openStream(
      _currSetup.outputStreamPtr(), _currSetup.inputStreamPtr(),
      RTAUDIO_FLOAT32, _currSetup.sampleRate(), _currSetup.bufferFramesPtr(),
      &RtSoundIO::onHandleStream, &_streamData, &_currSetup.streamOpts())};

  if (rterr == RTAUDIO_NO_ERROR) {
    notifyBeforeStartStream(_currSetup);
    _rta->startStream();
  }
}

void RtSoundIO::stopAudioStream() {
  if (_rta->isStreamRunning())
    _rta->stopStream();

  if (_rta->isStreamOpen())
    _rta->closeStream();
}

int RtSoundIO::onHandleStream(void *outputBuffer, void *inputBuffer,
                              unsigned int nFrames, double streamTime,
                              RtAudioStreamStatus streamStatus,
                              void *streamDataPtr) {

  auto &streamData = *static_cast<RtStreamData *>(streamDataPtr);
  streamData.output = static_cast<float *>(outputBuffer);
  streamData.input = static_cast<float *>(inputBuffer);
  streamData.nFrames = int(nFrames);
  streamData.streamTime = streamTime;

  const auto beginTime{std::chrono::high_resolution_clock::now()};
  streamData.soundIO->notifyReceiveStreamData(streamData);
  const auto endTime{std::chrono::high_resolution_clock::now()};
  const auto duration{std::chrono::duration_cast<std::chrono::microseconds>(
      endTime - beginTime)};

  streamData.soundIO->_streamInfo.exchange(
      {streamStatus, streamTime, duration.count()});

  return streamData.result;
}
