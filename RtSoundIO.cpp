#include "RtSoundIO.h"
#include <RtSoundClient.h>
#include <chrono>
#include <random>
#include <thread>

RtSoundIO::RtSoundIO() {}

void RtSoundIO::startSoundEngine(RtAudio::Api api) {
  stopSoundStream();

  _rta = std::make_shared<RtAudio>(api);
  _streamInfo._rta = _rta;
  _nextSetup.inputStream().deviceId = _rta->getDefaultInputDevice();
  _nextSetup.outputStream().deviceId = _rta->getDefaultOutputDevice();
  _currSetup = _nextSetup;

  notifyUpdateSoundDevices(listSoundDevices());
  notifyApplyStreamConfig(_currSetup);
}

void RtSoundIO::startSoundStream(bool shot) {
  stopSoundStream();
  orderClients();
  notifyConfigureStream(_nextSetup);

  _currSetup = _nextSetup;
  _currSetupPub = _currSetup;
  _streamData->setInputChannelsN(_currSetup.inputStream().nChannels);
  _streamData->setOutputChannelsN(_currSetup.outputStream().nChannels);
  _streamData->setResult(shot ? 1 : 0);

  const RtAudioErrorType rterr{_rta->openStream(
      _currSetup.outputStreamPtr(), _currSetup.inputStreamPtr(),
      RTAUDIO_FLOAT32, _currSetup.sampleRate(), _currSetup.bufferFramesPtr(),
      &RtSoundIO::onHandleStream, this, &_currSetup.streamOpts())};

  if (rterr == RTAUDIO_NO_ERROR) {
    notifyApplyStreamConfig(_currSetup);
    _rta->startStream();
  }
}

void RtSoundIO::stopSoundStream() {
  if (!_rta)
    return;

  if (_rta->isStreamRunning())
    _rta->stopStream();

  if (_rta->isStreamOpen())
    _rta->closeStream();
}

void RtSoundIO::applySoundSetup() { notifyApplyStreamConfig(_nextSetup); }

std::vector<RtAudio::DeviceInfo> RtSoundIO::listSoundDevices() const {
  std::vector<RtAudio::DeviceInfo> devices;
  const auto ids{_rta->getDeviceIds()};
  devices.reserve(ids.size());
  for (auto &id : ids) {
    devices.push_back(_rta->getDeviceInfo(id));
  }
  return devices;
}

int RtSoundIO::onHandleStream(void *outputBuffer, void *inputBuffer,
                              unsigned int nFrames, double streamTime,
                              RtAudioStreamStatus streamStatus, void *ioPtr) {

  auto &io = *static_cast<RtSoundIO *>(ioPtr);
  io._streamData->setOutput(static_cast<float *>(outputBuffer));
  io._streamData->setInput(static_cast<float *>(inputBuffer));
  io._streamData->setFramesN(int(nFrames));
  io._streamData->setStreamTime(streamTime);

  const auto beginTime{std::chrono::high_resolution_clock::now()};
  io.notifyStreamDataReady();
  const auto endTime{std::chrono::high_resolution_clock::now()};
  const auto duration{std::chrono::duration_cast<std::chrono::microseconds>(
      endTime - beginTime)};

  io._streamInfo.setStreamStatus(streamStatus);
  io._streamInfo.setStreamTime(streamTime);
  io._streamInfo.setProcessingTime(duration.count());
  io._streamInfo.setBufferTime(nFrames, io._currSetup.sampleRate());

  return io._streamData->result();
}
