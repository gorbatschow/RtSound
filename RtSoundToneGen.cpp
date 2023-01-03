#include "RtSoundToneGen.h"
#include <cmath>
#include <cstring>

void RtSoundToneGen::applyStreamConfig(const RtSoundSetup &setup) {
  _dt = setup.timeResolution();
  _df = setup.frequencyResolution();
  _fs = setup.sampleRate();
  _nwin = setup.bufferFrames();
}

void RtSoundToneGen::generate(float *buffer, int nFrames, double t) {
  const float f{std::round(float(_frequency.load() * _nwin) / 100.f / 2.f) *
                _df};
  const float A{float(_amplitude.load()) * _da};
  for (int i = 0; i != nFrames; ++i) {
    buffer[i] = A * cos(2 * M_PI * f * t);
    t += _dt;
  }
}
