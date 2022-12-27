#include "RtSoundToneGen.h"
#include <cmath>
#include <cstring>

RtSoundToneGen::RtSoundToneGen(int priority) : RtSoundBaseGen(priority) {}

RtSoundToneGen::~RtSoundToneGen() {}

void RtSoundToneGen::applyStreamConfig(const RtSoundSetup &setup) {
  _dt = setup.timeResolution();
  _df = float(setup.sampleRate()) / 2.f / 100.f;
}

void RtSoundToneGen::generate(float *buffer, int nFrames, double t) const {
  const float f{float(_frequency.load()) * _df};
  const float A{float(_amplitude.load()) * _da};
  for (int i = 0; i != nFrames; ++i) {
    buffer[i] = A * cos(2 * M_PI * f * t);
    t += _dt;
  }
}
