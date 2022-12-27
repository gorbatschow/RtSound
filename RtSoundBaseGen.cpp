#include "RtSoundBaseGen.h"
#include <cstring>

RtSoundBaseGen::RtSoundBaseGen(int priority) : RtSoundClient(priority) {}

void RtSoundBaseGen::receiveStreamData(const RtStreamData &streamData) {
  if (_inputEnabled) {
    std::lock_guard(streamData.mutex);
    fillInput(streamData);
  }

  if (_outputEnabled) {
    std::lock_guard(streamData.mutex);
    fillOutput(streamData);
  }
}

void RtSoundBaseGen::fillInput(const RtStreamData &streamData) const {
  if (_inputChannel >= 0) {
    generate(streamData.inputBuffer(_inputChannel), streamData.nFrames,
             streamData.streamTime);
  } else {
    generate(streamData.inputBuffer(0), streamData.nFrames,
             streamData.streamTime);
    for (int chan = 0; chan != streamData.nInputChannels; ++chan) {
      std::memcpy(streamData.inputBuffer(0), streamData.inputBuffer(chan),
                  streamData.nFrames);
    }
  }
}

void RtSoundBaseGen::fillOutput(const RtStreamData &streamData) const {
  if (_outputChannel >= 0) {
    generate(streamData.outputBuffer(_outputChannel), streamData.nFrames,
             streamData.streamTime);
  } else {
    generate(streamData.outputBuffer(0), streamData.nFrames,
             streamData.streamTime);
    for (int chan = 0; chan != streamData.nOutputChannels; ++chan) {
      std::memcpy(streamData.outputBuffer(0), streamData.outputBuffer(chan),
                  streamData.nFrames);
    }
  }
}
