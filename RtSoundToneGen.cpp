#include "RtSoundToneGen.h"
#include <cmath>

RtSoundToneGen::RtSoundToneGen() {}

void RtSoundToneGen::beforeStartStream(const RtSoundSetup &setup) {
  _dt = setup.timeResolution();
  _df = setup.frequencyResolution();
}

void RtSoundToneGen::receiveStreamData(const RtStreamData &streamData) {
  if (_inputEnabled) {
    auto *buffer = streamData.inputBuffer(_inputChannel);
    std::lock_guard(streamData.mutex);
    float t = streamData.info.streamTime;
    for (int i = 0; i != streamData.nFrames; ++i) {
      buffer[i] = std::cos(2 * 3.14 * 1000.0 * t);
      t += _dt;
    }
  }

  if (_outputEnabled) {
    auto buffer = streamData.outputBuffer(_outputChannel);
    std::lock_guard(streamData.mutex);
  }
}
