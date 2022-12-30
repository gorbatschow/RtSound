#include "RtSoundBaseGen.h"
#include <cstring>
#include <mutex>

RtSoundBaseGen::RtSoundBaseGen(int priority) : RtSoundClient(priority) {}

void RtSoundBaseGen::streamDataReady() {
  if (_inputEnabled) {
    fillInput(streamData());
  }

  if (_outputEnabled) {
    fillOutput(streamData());
  }
}

void RtSoundBaseGen::fillInput(const RtSoundData &data) const {
  std::unique_lock(data.mutex);
  if (data.inputsN() < 1 || data.inputsN() - 1 < _inputChannel) {
    return;
  }
  if (_inputChannel > 0) {
    generate(data.input(_inputChannel), data.framesN(), data.streamTime());
  } else {
    generate(data.input(0), data.framesN(), data.streamTime());
    for (int chan = 0; chan != data.inputsN(); ++chan) {
      std::memcpy(data.input(0), data.input(chan), data.framesN());
    }
  }
}

void RtSoundBaseGen::fillOutput(const RtSoundData &data) const {
  std::unique_lock lock(data.mutex);
  if (data.outputsN() < 1 || data.outputsN() - 1 < _outputChannel) {
    return;
  }
  if (_outputChannel > 0) {
    generate(data.output(_outputChannel), data.framesN(), data.streamTime());
  } else {
    generate(data.output(0), data.framesN(), data.streamTime());
    for (int chan = 0; chan != data.outputsN(); ++chan) {
      std::memcpy(data.output(0), data.output(chan), data.framesN());
    }
  }
}
