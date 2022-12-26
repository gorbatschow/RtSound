#include "RtSoundToneGen.h"
#include <cmath>

RtSoundToneGen::RtSoundToneGen(int priority) : RtSoundClient(priority) {}

RtSoundToneGen::~RtSoundToneGen() {}

void RtSoundToneGen::beforeStartStream(const RtSoundSetup &setup) {
  _dt = setup.timeResolution();
  _df = setup.frequencyResolution();
}

void RtSoundToneGen::receiveStreamData(const RtStreamData &streamData) {
  if (_inputEnabled) {
    std::lock_guard(streamData.mutex);
    if (_inputChannel >= 0) {
      generateTone(streamData.inputBuffer(_inputChannel), streamData.nFrames,
                   streamData.streamTime);
    } else {
      generateTone(streamData.inputBuffer(0), streamData.nFrames,
                   streamData.streamTime);
    }
  }
}

void RtSoundToneGen::generateTone(float *buffer, int nFrames, double t) {
  const float f{float(_frequencyIndex.load()) * _df};
  const float A{float(_amplitudeIndex.load()) * _da};
  for (int i = 0; i != nFrames; ++i) {
    buffer[i] = A * cos(2 * M_PI * f * t);
    t += _dt;
  }
}
