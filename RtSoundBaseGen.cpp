#include "RtSoundBaseGen.h"
#include <cstring>

RtSoundBaseGen::RtSoundBaseGen(int priority) : RtSoundClient(priority) {}

void RtSoundBaseGen::streamDataReady() {
  if (_inputEnabled) {
    fillInput(streamData());
  }

  if (_outputEnabled) {
    fillOutput(streamData());
  }
}

void RtSoundBaseGen::fillInput(const RtSoundData &streamData) const {
  std::lock_guard(streamData.mutex);
  if (_inputChannel >= 0) {
    generate(streamData.input(_inputChannel), streamData.framesN(),
             streamData.streamTime());
  } else {
    generate(streamData.input(0), streamData.framesN(),
             streamData.streamTime());
    for (int chan = 0; chan != streamData.inputsN(); ++chan) {
      std::memcpy(streamData.input(0), streamData.input(chan),
                  streamData.framesN());
    }
  }
}

void RtSoundBaseGen::fillOutput(const RtSoundData &streamData) const {
  std::lock_guard(streamData.mutex);
  if (_outputChannel >= 0) {
    generate(streamData.output(_outputChannel), streamData.framesN(),
             streamData.streamTime());
  } else {
    generate(streamData.output(0), streamData.framesN(),
             streamData.streamTime());
    for (int chan = 0; chan != streamData.outputsN(); ++chan) {
      std::memcpy(streamData.output(0), streamData.output(chan),
                  streamData.framesN());
    }
  }
}
