#include "RtSoundBaseGen.h"
#include <cstring>

RtSoundBaseGen::RtSoundBaseGen(int priority) : RtSoundClient(priority) {}

void RtSoundBaseGen::receiveStreamData(const RtSoundData &streamData) {
  if (_inputEnabled) {
    std::lock_guard(streamData.mutex);
    fillInput(streamData);
  }

  if (_outputEnabled) {
    std::lock_guard(streamData.mutex);
    fillOutput(streamData);
  }
}

void RtSoundBaseGen::fillInput(const RtSoundData &streamData) const {
  if (_inputChannel >= 0) {
    generate(streamData.inputBuffer(_inputChannel), streamData.framesN(),
             streamData.streamTime());
  } else {
    generate(streamData.inputBuffer(0), streamData.framesN(),
             streamData.streamTime());
    for (int chan = 0; chan != streamData.inputChannelsN(); ++chan) {
      std::memcpy(streamData.inputBuffer(0), streamData.inputBuffer(chan),
                  streamData.framesN());
    }
  }
}

void RtSoundBaseGen::fillOutput(const RtSoundData &streamData) const {
  if (_outputChannel >= 0) {
    generate(streamData.outputBuffer(_outputChannel), streamData.framesN(),
             streamData.streamTime());
  } else {
    generate(streamData.outputBuffer(0), streamData.framesN(),
             streamData.streamTime());
    for (int chan = 0; chan != streamData.outputChannelsN(); ++chan) {
      std::memcpy(streamData.outputBuffer(0), streamData.outputBuffer(chan),
                  streamData.framesN());
    }
  }
}
